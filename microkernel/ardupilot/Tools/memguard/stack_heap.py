#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

import sys
import re
import argparse
import os
import subprocess

elf = None
profile = None

dump = None

tasks = dict() # start address: task object

def find_symbol(addr):
    pipe = subprocess.Popen(["arm-none-eabi-addr2line", "-e", elf, \
                             "-i", "-p", "-s", "-f", \
                             # demangle C++ names: "-C", \
                             ("0x%x" % addr)],
                            stdout=subprocess.PIPE)
    stdout, _ = pipe.communicate()
    if pipe.returncode != 0:
        return None
    m = re.match(r"(.+) at (.+:\d+)$", stdout.rstrip())
    if m is None:
        return None
    return m.group(1)

class Task:
    def __init__(self, start):
        self.start = start
        self.stack = 0
        self.heap = 0

    def __str__(self):
        sym = find_symbol(self.start)
        if sym is None:
            return "%x: stack=%d heap=%d" % (self.start, self.stack, self.heap)
        return "%s: stack=%d heap=%d" % (sym, self.stack, self.heap)

    def set_stack(self, stack):
        self.stack = max(self.stack, stack)

    def set_heap(self, heap):
        self.heap = max(self.heap, heap)

def load_line(line):
    m = re.match(r"(\d+): start=([0-9a-f]+) stack=(\d+) heap=(\d+)$", line)
    if m is None:
        return False

    pid = int(m.group(1))
    start = int(m.group(2), 16)
    stack = int(m.group(3))
    heap = int(m.group(4))

    #print("[DBG] %d: start=%08x stack=%d heap=%d" % (pid, start, stack, heap))

    if start not in tasks:
        tasks[start] = Task(start)
    task = tasks[start]

    task.set_stack(stack)
    task.set_heap(heap)

def load_profile(f):
    for i, line in enumerate(f):
        load_line(line.rstrip())

def print_summary():
    if not print_summary:
        return

    for start, task in sorted(tasks.iteritems()):
        print(task)

def check_args(argv):
    global elf, profile
    global dump

    desc = "Summarize stack and heap profile"
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument("<elf>", type=str, help="ELF binary file")
    parser.add_argument("<profile>", type=argparse.FileType("r"),
                        help="Stack and heap profile printed by 'memguard_profile'")
    parser.add_argument("-p", "--print", dest="dump",
                        action="store_true",
                        help="Print summary in human-readable text")

    parser.set_defaults(dump=False)

    args = vars(parser.parse_args())
    elf = args["<elf>"]
    profile = args["<profile>"]
    dump = args["dump"]

def main():
    check_args(sys.argv)
    load_profile(profile)
    print_summary()

if __name__ == "__main__":
    main()
