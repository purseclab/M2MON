#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

import sys
import re
import argparse
import os
import ctypes

acl = None
mem = None

name = None

def check_range(start, end):
    if start <= mem[0] and mem[1] <= end:
        return True
    return False

def check_header(line):
    global name

    m = re.match(r"([^ ]+) \d+ \d+$", line)
    if m is None:
        return False

    name = m.group(1)

    #print("[DBG] " + name + ":")

    return True

def check_entry(line):
    m = re.match(r"\d+: ([0-9a-f]+)-([0-9a-f]+) [rwx]+$", line)
    if m is None:
        return False

    start = int(m.group(1), 16)
    end = int(m.group(2), 16)

    #print("[DBG] %08x-%08x" % (start, end))

    if check_range(start, end):
        print("%s: included" % (name))
        sys.exit(0)

    return True

def check_acl(f):
    for i, line in enumerate(f):
        if check_header(line.rstrip()):
            continue
        if check_entry(line.rstrip()):
            continue

def range_type(x):
    m = re.match(r"(0x)?([0-9a-fA-F]+)-(0x)?([0-9a-fA-F]+)", x)
    if m is None:
        raise ValueError()
    start = int(m.group(2), 16)
    end = int(m.group(4), 16)
    return (start, end)

def check_args(argv):
    global acl, mem

    desc = "Check if an ACL includes a specified memory region in it."
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument("<acl>", type=argparse.FileType("r"),
                        help="ACL built by 'build_acl.py'")
    parser.add_argument("<start-end>", type=range_type,
                        help="Address range of a memory region to check")

    args = vars(parser.parse_args())
    acl = args["<acl>"]
    mem = args["<start-end>"]

def main():
    check_args(sys.argv)
    check_acl(acl)
    print("%s: not included" % (name))

if __name__ == "__main__":
    main()
