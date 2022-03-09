#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

import sys
import re
import argparse
import os
import ctypes

bitcode = None

print_chain = None
print_verbose = None

functions = list()
current_function = None

global_vars = dict() # name: global variable

class GlobalVar:
    def __init__(self, name, scope, type):
        self.name = name
        self.scope = scope
        self.type = type

    def __str__(self):
        return "%s = %s %s" % (self.name, self.scope, self.type)

class Function:
    def __init__(self, name):
        self.name = name
        self.labels = set()
        self.uses = dict()

    def __str__(self):
        return self.name

class Use:
    def __init__(self, var):
        self.var = var
        self.mode = ""

    def __str__(self):
	return "%s %-2s" % (self.var.name, self.mode)

    def read(self):
        if "r" in self.mode:
            return
        if "w" in self.mode:
            self.mode = "rw"
            return
        self.mode = "r"

    def write(self):
        if "w" in self.mode:
            return
        if "r" in self.mode:
            self.mode = "rw"
            return
        self.mode = "w"

def global_var_decl(line, lineno):
    m = re.match(r"(@[a-zA-Z_][a-zA-Z_0-9]*) = (.*global) (.*)$", line)
    if m is None:
            return False
    name = m.group(1)
    scope = m.group(2)
    assert(scope is not None)
    type = m.group(3)

    # TEST
    if name != "@optind":
        return True

    assert(name not in global_vars)
    global_vars[name] = GlobalVar(name, scope, type)

    if print_chain:
        if print_verbose:
            print("# " + str(lineno) + ": " + line)
            print("  " + str(var))
        #print(var)

    return True

def function_open(line, lineno):
    global current_function

    m = re.match(r"define [^@]+ @([^(]+)\(.*\).*{", line)
    if m is None:
        return False
    name = m.group(1)

    # Ignore cloned functions
    if re.match(r".+\.\d+$", name) is not None:
        return True

    current_function = Function(name)

    functions.append(current_function)

    if print_chain:
        if print_verbose:
            print("# " + str(lineno) + ": " + line)
            print(str(current_function) + ":")

    return True

def function_close(line, lineno):
    global current_function

    if line != "}":
        return False

    if current_function is None:
        return True

    current_function = None

    if print_chain:
        if print_verbose:
            print("")

    return True

def is_used(label, line):
    if label in line:
        return True
    return False

def get_result(line):
    m = re.match(r"(%[a-zA-Z_0-9]*) = .*$", line)
    if m is None:
        return None
    res = m.group(1)
    return res

def store_global_var(line, lineno, var):
    m = re.match(r"store.*, .* " + var.name + ".*$", line)
    if m is None:
        return False
    #print("[DBG] stored: " + var.name)

    uses = current_function.uses
    if var not in uses:
        uses[var] = Use(var)
    use = uses[var]
    use.write()

    return True

#def call(line, lineno, var):
#    m = re.match(r"(%[a-zA-Z_0-9]* = )?call .*$", line)
#    if m is None:
#        return False
#    res = m.group(1)
#    return True

def use(line, lineno, var):
    if current_function is None:
        return True

    labels = current_function.labels
    uses = current_function.uses

    if var.name not in labels: # add the global var to the set if we have not
        labels.add(var.name)

    for label in list(labels):
        if not is_used(label, line): # is label unused?
            continue
        #print("[DBG] " + line)
        #print("[DBG] used: " + label)
        res = get_result(line)
        if res is not None:
            #print("[DBG] res: " + res)
            if res not in labels:
                labels.add(res)
        if store_global_var(line, lineno, var):
            continue
        #if call(line, lineno, var):
        #    continue
        if var not in uses:
            uses[var] = Use(var)
        use = uses[var]
        use.read()

    return True

def print_du_chain():
    if not print_chain:
        return
    for function in functions:
        if len(function.uses) == 0:
            continue
        print("Function: '" + str(function) + "'")
        for use in function.uses.itervalues():
            print("  " + str(use))
        print("")

    #print("# Total %d global variables" % (len(global_vars)))
    #for name, var in sorted(global_vars.iteritems()):
    #    print(var)

def check_args(argv):
    global bitcode
    global print_chain
    global print_verbose

    desc = "Build a Definition-Use Chain on every global variable"
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument("<bitcode>", type=argparse.FileType("r"),  
                        help="LLVM bitcode disassembled by 'llvm-dis'")
    parser.add_argument("-c", "--print-chain", dest="print_chain", 
                        action="store_true", 
                        help="Print every DU Chain (default: enabled)")
    parser.add_argument("-v", "--print-verbose", dest="print_verbose", 
                        action="store_true", 
                        help="Print verbose messages")

    parser.set_defaults(print_chain=False)
    parser.set_defaults(print_verbose=False)

    args = vars(parser.parse_args())
    bitcode = args["<bitcode>"]
    print_chain = args["print_chain"]
    print_verbose = args["print_verbose"]

    if not print_chain:
        print_chain = True

def main():
    check_args(sys.argv)

    for i, line in enumerate(bitcode):
        line = line.rstrip()
        if global_var_decl(line, i+1):
            continue

    for var in global_vars.itervalues():
        bitcode.seek(0, 0)
        for i, line in enumerate(bitcode):
            line = line.strip()
            if function_open(line, i+1):
                continue
            if function_close(line, i+1):
                continue
            if use(line, i+1, var):
                continue

    current_function = None

    print_du_chain()

if __name__ == "__main__":
    main()
