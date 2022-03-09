#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

import re
import subprocess

callgraph = dict() # node name: node
root_node = None
current_node = None

class Node:
    def __init__(self, name):
        self.name = name
        self.callees = dict()
        self.callers = dict()

    def __str__(self):
        return self.name

def demangle(name):
    if name is None:
        return None
    pipe = subprocess.Popen("c++filt",
                            stdin=subprocess.PIPE,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)
    stdout, _ = pipe.communicate(name)
    return stdout.rstrip()

def node_define(line, lineno):
    m = re.match(r"Call graph node for function: '([^']+)'(<<0x[0-9a-f]+>>)?"
                 r"  #(uses|callers)=\d+", line)
    if m is None:
        return False

    name = m.group(1)

    if name in callgraph:
        return True

    node = Node(name)
    callgraph[name] = node

    #print("# " + str(lineno) + ": " + line)
    #print(str(node))

    return True

def node_root(line, lineno):
    global root_node

    m = re.match(r"CallGraph Root is: '([^']+)'", line)
    if m is None:
        return False

    name = m.group(1)

    assert(root_node is None)
    assert(name in callgraph)
    root_node = callgraph[name]

    #print("# " + str(lineno) + ": " + line)
    #print("# Root: " + str(root_node))
    #print("")

    return True

def node_open(line, lineno):
    global current_node

    m = re.match(r"Call graph node for function: '([^']+)'(<<0x[0-9a-f]+>>)?"
                 r"  #(uses|callers)=\d+", line)
    if m is None:
        return False

    name = m.group(1)

    assert(name in callgraph)
    current_node = callgraph[name]

    #print("# " + str(lineno) + ": " + line)
    #print("# Node: " + str(current_node))

    return True

def node_close(line, lineno):
    global current_node

    if line != "":
        return False

    if current_node is None:
        return True

    current_node = None

    #print("")

    return True

def node_callsite(line, lineno, blacklist):
    m = re.match(r"  (CS<0x[0-9a-f]+> )?[cC]alls function '([^']+)'", line)
    if m is None:
        return False

    if current_node is None:
        return True

    name = m.group(2)

    # Ignore blacklisted functions
    if blacklist is not None and name in blacklist:
        return True

    if name not in callgraph:
        print("Error: '" + name + "'")
    assert(name in callgraph)
    callee = callgraph[name]

    if name not in current_node.callees:
        current_node.callees[name] = callee

    if current_node.name not in callee.callers:
        callee.callers[current_node.name] = current_node

    #print("# " + str(lineno) + ": " + line)
    #print("#   Call: " + str(callee))

    return True

def load(f, blacklist):
    #print("## CallGraph: " + f.name)

    for i, line in enumerate(f):
        line = line.rstrip()
        if node_define(line, i+1):
            continue

    f.seek(0)
    for i, line in enumerate(f):
        line = line.rstrip()
        if node_root(line, i+1):
            continue
        if node_open(line, i+1):
            continue
        if node_close(line, i+1):
            continue
        if node_callsite(line, i+1, blacklist):
            continue

def remove_node(node):
    if node.name not in callgraph:
        return
    for caller in node.callers.itervalues():
        del caller.callees[node.name]
    for callee in node.callees.itervalues():
        del callee.callers[node.name]
    del callgraph[node.name]

def print_node(node):
    print("Call graph node for function: '" + node.name + "'  #callers=" +
          str(len(node.callers)))
    #for caller in node.callers.itervalues():
    #    print("  Called by function '" + caller.name + "'")
    for callee in node.callees.itervalues():
        print("  Calls function '" + callee.name + "'")
    print("")

def dot_node(node):
    name = demangle(node.name)
    name = name.replace("\"", "")
    name = name.replace("<", "\\<")
    name = name.replace(">", "\\>")
    name = name.replace("-\\>", "->")
    name = name.replace("{", "\\{")
    name = name.replace("}", "\\}")
    print("    " + ("Node0x%x" % id(node)) + " [shape=record,label=\"{" +
          name + "}\"];")
    for callee in node.callees.itervalues():
        print("    " + ("Node0x%x" % id(node)) + " -> " +
             ("Node0x%x" % id(callee)) + ";")

def print_callgraph():
    if root_node is not None:
        print("CallGraph Root is: '" + root_node.name + "'")
        print("")
    for name, node in sorted(callgraph.iteritems()):
        print_node(node)
    print("# Number of nodes: " + str(len(callgraph)))

def dot_callgraph():
    print("digraph \"Call graph\" {")
    print("    label=\"Call graph\";")
    print("")
    for name, node in sorted(callgraph.iteritems()):
        dot_node(node)
    print("}")
