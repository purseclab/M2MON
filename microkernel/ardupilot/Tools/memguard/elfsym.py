#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

import sys
import re
import argparse
import os
import subprocess

elf = None
sym = None

class Symbol:
    def __init__(self, addr, size, type, bind, vis, ndx, name):
        self.addr = addr
        self.size = size
        self.type = type
        self.bind = bind
        self.vis = vis
        self.ndx = ndx
        self.name = name

    def __str__(self):
        return ("%08x %6d %6s %6s %7s %3s %s" % 
                (self.addr, self.size, self.type, self.bind, self.vis, self.ndx, self.name))

def get_symbol_table(elf):
    pipe = subprocess.Popen(["arm-none-eabi-readelf", "-W", "-s", elf], 
                            stdout=subprocess.PIPE)
    stdout, _ = pipe.communicate()
    if pipe.returncode != 0:
        return None
    return stdout.rstrip()

def load_symbol_table(elf):
    s = get_symbol_table(elf)
    if s is None:
        return None

    symtbl = dict() # name: symbols

    for line in s.splitlines():
        m = re.match(r"[ ]+\d+:[ ]+([0-9a-f]+)[ ]+(\d+)[ ]+"
                     r"(NOTYPE|FUNC|OBJECT)[ ]+(GLOBAL|LOCAL|WEAK)[ ]+"
                     r"(DEFAULT|HIDDEN)[ ]+(\d+|ABS|UND)[ ]+(.+)", line)
        if m is None:
            continue

        addr = int(m.group(1), 16)
        size = int(m.group(2))
        type = m.group(3)
        bind = m.group(4)
        vis = m.group(5)
        ndx = m.group(6)
        name = m.group(7)

        symbol = Symbol(addr, size, type, bind, vis, ndx, name)

        if name not in symtbl:
            symtbl[name] = list()
        symtbl[name].append(symbol)

    return symtbl

def find_symbols(elf, sym):
    symtbl = load_symbol_table(elf)
    if symtbl is None:
        return None
    if sym not in symtbl:
        return None
    return symtbl[sym]

def check_args(argv):
    global elf, sym
    desc = "Print the information of an ELF symbol"
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument("<elf>", type=str, 
                        help="ELF binary file")
    parser.add_argument("<sym>", type=str, 
                        help="ELF symbol name")
    args = vars(parser.parse_args())
    elf = args["<elf>"]
    sym = args["<sym>"]

def main():
    check_args(sys.argv)

    symbols = find_symbols(elf, sym)
    if symbols is None:
        sys.stderr.write("Error: could not find symbol '" + sym + "'" + 
                         os.linesep)
        sys.exit(1)

    print("%8s %6s %6s %6s %7s %3s %s" % 
          ("Address", "Size", "Type", "Bind", "Vis", "Ndx", "Name"))
    for symbol in symbols:
        print(symbol)

if __name__ == "__main__":
    main()
