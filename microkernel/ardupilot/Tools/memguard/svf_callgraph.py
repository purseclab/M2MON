#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

import sys
import argparse
import os
from collections import deque
import callgraph as c

svf_callgraph = None

entry_name = None

print_callgraph = None
dot_callgraph = None
print_verbose = None

def find_reachable_nodes(entry):
    # BFS graph traveral
    nodes = list()
    visited = set()
    queue = deque()
    queue.append(entry)
    while len(queue) > 0:
        node = queue.popleft()
        if node in visited:
            continue
        visited.add(node)
        nodes.append(node)
        for callee in node.callees.itervalues():
            if callee in visited:
                continue
            queue.append(callee)
    return nodes

def print_reachable_callgraph(entry):
    nodes = find_reachable_nodes(entry)
    print("CallGraph Root is: '" + entry.name + "'")
    print("")
    for node in nodes:
        c.print_node(node)
    print("# Number of nodes: " + str(len(c.callgraph)))
    print("# Number of reachable nodes from '" + entry.name + "': " + 
          str(len(nodes)) + " (" + 
          ("%.2f" % (float(len(nodes))*100/len(c.callgraph))) + "%)")

def dot_reachable_callgraph(entry):
    nodes = find_reachable_nodes(entry)
    print("digraph \"Call graph\" {")
    print("    label=\"Call graph\";")
    print("")
    for node in nodes:
        c.dot_node(node)
    print("}")

def check_args(argv):
    global svf_callgraph
    global entry_name
    global print_callgraph, dot_callgraph
    global print_verbose

    desc = "Build a call graph that includes both direct and indirect calls"
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument("<svf-callgraph>", type=argparse.FileType("r"),  
                        help="Call graph dumped by 'saber -cgraph'")
    parser.add_argument("-e", "--entry", dest="FUNCTION", type=str,  
                        help="Build a call graph with only the reachable " 
                             "functions from entry FUNCTION")
    parser.add_argument("-c", "--print-callgraph", dest="print_callgraph", 
                        action="store_true", 
                        help="Print a call graph (default: enabled)")
    parser.add_argument("-d", "--dot-callgraph", dest="dot_callgraph", 
                        action="store_true", 
                        help="Print a call graph in 'dot' format")
    parser.add_argument("-v", "--print-verbose", dest="print_verbose", 
                        action="store_true", 
                        help="Print verbose messages")

    parser.set_defaults(print_callgraph=False)
    parser.set_defaults(dot_callgraph=False)
    parser.set_defaults(print_verbose=False)

    args = vars(parser.parse_args())
    svf_callgraph = args["<svf-callgraph>"]
    entry_name = args["FUNCTION"]
    print_callgraph = args["print_callgraph"]
    dot_callgraph = args["dot_callgraph"]
    print_verbose = args["print_verbose"]

    if not print_callgraph and \
       not dot_callgraph:
        print_callgraph = True

def main():
    check_args(sys.argv)

    c.load(svf_callgraph, None)

    # Build a call graph with only the reachable nodes from the specified entry
    entry = None
    if entry_name is not None:
        if entry_name not in c.callgraph:
            sys.stderr.write("Error: function '" + entry_name + 
                             "' is not found in the call graph" + os.linesep)
            sys.exit(1)
        entry = c.callgraph[entry_name]

    if print_callgraph:
        if entry is None:
            c.print_callgraph()
        else:
            print_reachable_callgraph(entry)
    elif dot_callgraph:
        if entry is None:
            c.dot_callgraph()
        else:
            dot_reachable_callgraph(entry)

if __name__ == "__main__":
    main()
