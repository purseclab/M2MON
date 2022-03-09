#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

import sys
import re
import argparse
import os
import numpy as np
from sklearn.cluster import KMeans
import elfsym
import callgraph as c
from acl import *

N = 7
STACK_HEAP_START = 0x10000000

elf = None
callgraph_file = None
global_file = None
mmio_file = None
stack_heap_file = None

code_size = 0
global_size = 0
mmio_size = 0
stack_size = 0
heap_size = 0

print_accesses = None
print_callgraph = None
print_verbose = None

symtbl = None
current_function = None

shadow_memory = dict() # address: permission

acl = dict() # address: access control entry

def sym_name(name):
    if name == "_ZN3PX412PX4Scheduler15_storage_threadEPv":
        return "_ZN3PX412PX4Scheduler15_storage_threadEv"
    if name == "_ZN3PX412PX4Scheduler12_uart_threadEPv":
        return "_ZN3PX412PX4Scheduler12_uart_threadEv"
    if name == "_ZN3PX412PX4Scheduler13_timer_threadEPv":
        return "_ZN3PX412PX4Scheduler13_timer_threadEv"
    if name == "_ZN3PX412PX4Scheduler10_io_threadEPv":
        return "_ZN3PX412PX4Scheduler10_io_threadEv"
    if name == "_ZN3PX414NSHShellStream12shell_threadEPv":
        return "_ZN3PX414NSHShellStream12shell_threadEv"
    m = re.match(r"([^\.]+)\.\d+.*$", name)
    if m is None:
        return name
    return  m.group(1)

def find_sym(name, deref_name):
    if sym_name(deref_name) not in symtbl:
        sys.stderr.write("Error: '" + deref_name +
                         "' is not in the symbol table" + os.linesep)
        return None

    deref_syms = symtbl[sym_name(deref_name)]
    deref_sym = deref_syms[0]

    if sym_name(name) not in symtbl:
        if print_verbose:
            sys.stderr.write("Warning: '" + name +
                             "' is not in the symbol table" + os.linesep)
        return None

    # choose the closest symbol in the memory space to the deref. symbol
    min_d = 0xffffffff
    min_sym = None
    syms = symtbl[sym_name(name)]
    for sym in syms:
        if sym.addr == 0:
            continue
        d = abs(sym.addr - deref_sym.addr)
        if min_d > d:
            min_d = d
            min_sym = sym

    return min_sym

def round_up(addr):
    remainder = addr % 32
    if remainder == 0:
        return addr
    return addr + 32 - remainder

def round_down(addr):
    return addr & ~31

def load_callgraph(f):
    global code_size

    c.load(f, None)

    entry = c.root_node
    if entry is None:
        sys.stderr.write("Error: call graph root is not specified in '" + 
                         f.name + "'" + os.linesep)
        sys.exit(1)

    if sym_name(entry.name) not in symtbl:
        sys.stderr.write("Error: entry function '" + entry.name + 
                         "' is not in the symbol table" + os.linesep)
        sys.exit(1)

    to_remove = list()
    for name, node in c.callgraph.iteritems():
        # Remove functions not in the symbol table
        #if sym_name(name) not in symtbl:
        #    if print_verbose:
        #        sys.stderr.write("Warning: function '" + name + 
        #                         "' is not in the symbol table" + os.linesep)
        #    to_remove.append(node)
        #    continue

        #syms = symtbl[name]

        # Remove functions without valid address
        #remove = True
        #for sym in syms:
        #    if sym.addr != 0:
        #        remove = False
        #        break
        #if remove:
        #    to_remove.append(node)
        #    continue

        sym = find_sym(name, entry.name)
        if sym is None or sym.addr == 0:
            to_remove.append(node)
            continue

        start = round_down(sym.addr)
        end = round_up((sym.addr & ~1) + sym.size)

        # Mark the access in the shadow memory
        #for sym in syms:
        #    if sym.addr == 0:
        #        continue
        perm = Permission("rx")
        for addr in range(start, end):
            if addr not in shadow_memory:
                shadow_memory[addr] = perm
                code_size = code_size + 1
                continue
            if shadow_memory[addr] != perm:
                shadow_memory[addr] = shadow_memory[addr] + perm

    sys.stderr.write("Warning: removing " + str(len(to_remove)) + 
                     " functions not in the symbol table..." + os.linesep)

    for node in to_remove:
        c.remove_node(node)

    if print_callgraph:
        c.print_callgraph()

def function_open(line, lineno):
    global current_function

    m = re.match(r"Function: '([^']+)'", line)
    if m is None:
        return False

    name = m.group(1)

    if name not in c.callgraph:
        if print_verbose:
            sys.stderr.write("Warning: function '" + name + 
                             "' is not in the call graph" + os.linesep)
        return True

    current_function = name

    #print(name + ":")

    return True

def function_close(line, lineno):
    global current_function

    if line != "":
        return False

    if current_function is None:
        return True

    current_function = None

    #print("")

    return True

def global_access(line, lineno):
    global global_size

    if current_function is None:
        return True

    if current_function not in c.callgraph:
        return True

    m = re.match(r"  (.+) (rw|w|r)$", line)
    if m is None:
        return False

    name = m.group(1)
    perm = Permission(m.group(2))

    #print("  " + ("%s %-3s" % (name, str(perm))))

    sym = find_sym(name, current_function)
    if sym is None or sym.addr == 0:
        return

    start = round_down(sym.addr)
    end = round_up((sym.addr & ~1) + sym.size)

    # Mark the access in the shadow memory
    for addr in range(start, end):
        if addr not in shadow_memory:
            shadow_memory[addr] = perm
            global_size = global_size + 1
            continue
        if shadow_memory[addr] != perm:
            shadow_memory[addr] = shadow_memory[addr] + perm

    return True

def mmio_access(line, lineno):
    global mmio_size

    if current_function is None:
        return True

    if current_function not in c.callgraph:
        return True

    m = re.match(r"  ([0-9a-f]+)-([0-9a-f]+) (rw|w|r)([ ]*#.+)?", line)
    if m is None:
        return False

    start = int(m.group(1), 16)
    end = int(m.group(2), 16)
    perm = Permission(m.group(3))

    assert(start < end)

    #print("  " + ("%08x-%08x %-3s" % (start, end, str(perm))))

    # Mark the access in the shadow memory
    for addr in range(start, end):
        if addr not in shadow_memory:
            shadow_memory[addr] = perm
            mmio_size = mmio_size + 1
        if shadow_memory[addr] != perm:
            shadow_memory[addr] = shadow_memory[addr] + perm

    return True

def stack_heap_size(line, lineno):
    global stack_size, heap_size

    m = re.match(r"(.+): stack=(\d+) heap=(\d+)$", line)
    if m is None:
        return False

    main = m.group(1)
    stack = int(m.group(2))
    heap = int(m.group(3))

    #print("  " + ("%s: stack=%d heap=%d" % (main, stack, heap)))

    entry = c.root_node
    assert(entry is not None)

    if main == "main_loop":
        main = "_ZL9main_loopiPPc"

    if sym_name(entry.name) != main:
        return True

    start = STACK_HEAP_START
    end = start + stack
    perm = Permission("rw")

    # Mark the access in the shadow memory
    for addr in range(start, end):
        if addr not in shadow_memory:
            shadow_memory[addr] = perm
            stack_size = stack_size + 1
            continue
        if shadow_memory[addr] != perm:
            shadow_memory[addr] = shadow_memory[addr] + perm

    start = end
    end = start + heap
    perm = Permission("rw")

    # Mark the access in the shadow memory
    for addr in range(start, end):
        if addr not in shadow_memory:
            shadow_memory[addr] = perm
            heap_size = heap_size + 1
            continue
        if shadow_memory[addr] != perm:
            shadow_memory[addr] = shadow_memory[addr] + perm

    return True

def load_global(f):
    for i, line in enumerate(f):
        line = line.rstrip()
        if function_open(line, i+1):
            continue
        if function_close(line, i+1):
            continue
        if global_access(line, i+1):
            continue

def load_mmio(f):
    for i, line in enumerate(f):
        line = line.rstrip()
        if function_open(line, i+1):
            continue
        if function_close(line, i+1):
            continue
        if mmio_access(line, i+1):
            continue

def load_stack_heap(f):
    for i, line in enumerate(f):
        line = line.rstrip()
        if stack_heap_size(line, i+1):
            continue

def max_permission(addrs):
    perm = Permission(0)
    for addr in addrs:
        perm = perm + shadow_memory[addr]
    return perm

def build_acl(N):
    addrs = sorted(shadow_memory.iterkeys())

    if N == 0:
        start = addrs[0]
        for i in range(1, len(addrs)):
            addr = addrs[i]
            prev = addrs[i-1]
            if prev+1 == addr and \
               shadow_memory[prev].mod == shadow_memory[addr].mod:
                continue
            perm = Permission(shadow_memory[start].mod)
            acl[start] = ACE(start, prev+1-start, perm, None)
            start = addr
        return

    X = np.empty([len(shadow_memory), 2], dtype=int)
    for i in range(len(addrs)):
        X[i][0] = addrs[i]
        X[i][1] = shadow_memory[addrs[i]].mod

    kmeans = KMeans(n_clusters=N)
    kmeans.fit(X)

    #centroids = kmeans.cluster_centers_
    labels = kmeans.labels_

    addrtbl = [None] * N
    for i in range(len(labels)):
        label = labels[i]
        if addrtbl[label] is None:
            addrtbl[label] = list()
        addrtbl[label].append(X[i][0])

    for i in range(len(addrtbl)):
        addrlist = addrtbl[i]
        if addrlist is None:
            continue
        start = min(addrlist)
        end = max(addrlist)+1
        perm = max_permission(addrlist)
        acl[start] = ACE(start, end-start, perm, len(addrlist))

def print_acl():
    if not print_accesses:
        return
    assert(c.root_node is not None)
    entry = c.root_node.name
    assert(sym_name(entry) in symtbl)
    syms = symtbl[sym_name(entry)]
    print("%s %d %d" % (sym_name(entry), len(syms), len(acl)))
    for sym in syms:
        print("%08x" % sym.addr)
    access_size = 0
    i = 0
    for addr, access in sorted(acl.iteritems()):
        print(str(i) + ": " + str(access))
        access_size = access_size + (access.size)
        i = i + 1
    over_approx = max(0, access_size - (code_size + global_size + mmio_size + \
                                        stack_size + heap_size))
    print("# code: %d" % (code_size))
    print("# global: %d" % (global_size))
    print("# mmio: %d" % (mmio_size))
    print("# stack: %d" % (stack_size))
    print("# heap: %d" % (heap_size))
    print("# over-approximated: %d" % (over_approx))

def check_args(argv):
    global elf, callgraph_file, global_file, mmio_file, stack_heap_file
    global N
    global print_accesses, print_callgraph
    global print_verbose

    desc = "Build an access control list using analysis results"
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument("<elf>", type=str,
                        help="ELF binary file")
    parser.add_argument("<callgraph>", type=argparse.FileType("r"),
                        help="Call graph printed by 'llvm_callgraph.py'")
    parser.add_argument("<global>", type=argparse.FileType("r"),
                        help="Global accesses identified by 'saber -minion'")
    parser.add_argument("<mmio>", type=argparse.FileType("r"),
                        help="MMIO accesses identified by 'llvm_mmio.py'")
    parser.add_argument("<stack_heap>", type=argparse.FileType("r"),
                        help="Stack and heap profile analyzed by stack_heap.py")
    parser.add_argument("-n", "--size", dest="N", type=int,
                        help="Limit the size of an access control list to N "
                             "(default: " + str(N) + ")")
    parser.add_argument("-a", "--print-acl", dest="print_acl",
                        action="store_true",
                        help="Print an access control list (default: enabled)")
    parser.add_argument("-c", "--print-callgraph", dest="print_callgraph",
                        action="store_true",
                        help="Print a call graph with only functions in the "
                             "symbol table")
    parser.add_argument("-v", "--print-verbose", dest="print_verbose", 
                        action="store_true", 
                        help="Print verbose messages")

    parser.set_defaults(N=N)
    parser.set_defaults(print_acl=False)
    parser.set_defaults(print_callgraph=False)
    parser.set_defaults(print_verbose=False)

    args = vars(parser.parse_args())
    elf = args["<elf>"]
    callgraph_file = args["<callgraph>"]
    global_file = args["<global>"]
    mmio_file = args["<mmio>"]
    stack_heap_file = args["<stack_heap>"]
    N = args["N"]
    print_accesses = args["print_acl"]
    print_callgraph = args["print_callgraph"]
    print_verbose = args["print_verbose"]

    if not print_accesses and \
       not print_callgraph:
        print_accesses = True

def main():
    global symtbl

    check_args(sys.argv)

    symtbl = elfsym.load_symbol_table(elf)
    if symtbl is None:
        sys.exit(1)

    load_callgraph(callgraph_file)
    load_global(global_file)
    load_mmio(mmio_file)
    load_stack_heap(stack_heap_file)

    build_acl(N)
    print_acl()

if __name__ == "__main__":
    main()
