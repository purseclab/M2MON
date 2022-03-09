#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

import sys
import re
import argparse
import os
from acl import *

acl_files = None

print_accesses = None
print_clang = None
print_verbose = None

acls = dict() # entry address: access control list
current_acl = None
addr_count = 0
ace_count = 0

def acl_header(line, fname, lineno):
	global current_acl, addr_count, ace_count

	m = re.match(r"([^ ]+) (\d+) (\d+)$", line)
	if m is None:
		return False

	assert(addr_count == 0)
	assert(ace_count == 0)

	name = m.group(1)
	addr_count = int(m.group(2))
	ace_count = int(m.group(3))

	assert(name not in acls)
	current_acl = ACL(name)
	acls[name] = current_acl

	if print_accesses:
		if print_verbose:
			print("# " + fname + ":" + str(lineno) + ": " + line)
			print("%s %d %d" % (name, addr_count, ace_count))

	return True

def acl_addr(line, fname, lineno):
	global addr_count

	m = re.match(r"([0-9a-f]+)$", line)
	if m is None:
		return False
	
	addr = int(m.group(1), 16)

	assert(current_acl is not None)
	assert(addr_count > 0)

	current_acl.addrs.append(addr)

	addr_count = addr_count - 1

	if print_accesses:
		if print_verbose:
			print("# " + fname + ":" + str(lineno) + ": " + line)
			print("%08x" % addr)

	return True

def acl_ace(line, fname, lineno):
	global ace_count

	m = re.match(r"(\d+): ([0-9a-f]+)-([0-9a-f]+) ([rwx]+)[ ]*?(#.*)?$", line)
	if m is None:
		return False
	
	index = int(m.group(1))
	start = int(m.group(2), 16)
	end = int(m.group(3), 16)
	perm = Permission(m.group(4))

	assert(current_acl is not None)
	assert(ace_count > 0)

	ace = ACE(start, end-start, perm, None)
	current_acl.aces.append(ace)

	ace_count = ace_count - 1

	if print_accesses:
		if print_verbose:
			print("# " + fname + ":" + str(lineno) + ": " + line)
			print("%d: %08x-%08x %s" % (index, start, end, str(perm)))

	return True

def load_acl(f):
	for i, line in enumerate(f):
		line = line.rstrip()
		if acl_header(line, f.name, i+1):
			continue
		if acl_addr(line, f.name, i+1):
			continue
		if acl_ace(line, f.name, i+1):
			continue

def load_acls():
	for f in acl_files:
		load_acl(f)

def print_acls():
	if not print_accesses:
		return
	tbl = dict() # entry address: access control list
	for acl in acls.itervalues():
		for addr in acl.addrs:
			assert(addr not in tbl)
			tbl[addr] = acl
	for addr, acl in sorted(tbl.iteritems()):
		size = 0
		print("%08x: %s" % (addr, acl.name))
		for i in range(len(acl.aces)):
			print("%d: %s" % (i, acl.aces[i]))
			size = size + acl.aces[i].size
		print("# Total bytes: " + str(size))
		print("")
	print("# Number of ACLs: " + str(len(tbl)))

def print_code():
	if not print_clang:
		return
	tbl = dict() # entry address: access control list
	for acl in acls.itervalues():
		for addr in acl.addrs:
			assert(addr not in tbl)
			tbl[addr] = acl
	print("/* Auto-generated: %s */" % sys.argv[0])
	print("")
	print("/****************************************************************************")
	print(" * Included Files")
	print(" ****************************************************************************/")
	print("")
	print("#include <nuttx/config.h>")
	print("")
	print("#include <nuttx/memguard.h>")
	print("")
	print("#ifdef CONFIG_MEMGUARD")
	print("")
	print("/****************************************************************************")
	print(" * Public Data")
	print(" ****************************************************************************/")
	print("")
	print("const volatile memguard_acl_t memguard_acl g_acls[%d] = {" \
		  % len(tbl))
	for i, (addr, acl) in enumerate(sorted(tbl.iteritems())):
		print("  { .start = 0x%08x, .sym = \"%s\"," % (addr, acl.name))
		print("    .segments = (memguard_segment_t[%d]){" % len(acl.aces))
		for j, ace in enumerate(acl.aces):
			print("      { .base = 0x%08x, .size = 0x%06x, .mod = %d }%s" % \
				  (ace.addr, ace.size, ace.perm.mod, 
				   ("," if j < len(acl.aces)-1 else " }")))
		print("  }%s" % ("," if i < len(tbl)-1 else ""))
	print("};")
	print("")
	print("const volatile uint8_t g_aclcount = sizeof(g_acls)/sizeof(g_acls[0]);")
	print("")
	print("#endif /* CONFIG_MEMGUARD */")

def check_args(argv):
	global acl_files
	global print_accesses, print_clang
	global print_verbose

	desc = "Merge multiple access control lists"
	parser = argparse.ArgumentParser(description=desc)
	parser.add_argument("<acl>", type=argparse.FileType("r"), nargs="+", 
						help="Access control list printed by 'build_acl.py'")
	parser.add_argument("-l", "--print-acls", dest="print_acls", 
						action="store_true", 
						help="Print access control lists in human-readable "
							 "text")
	parser.add_argument("-c", "--print-clang", dest="print_clang", 
						action="store_true", 
						help="Print access control lists in C language"
							 "(default: enabled)")
	parser.add_argument("-v", "--print-verbose", dest="print_verbose", 
						action="store_true", 
						help="Print verbose messages")

	parser.set_defaults(print_acls=False)
	parser.set_defaults(print_clang=False)
	parser.set_defaults(print_verbose=False)

	args = vars(parser.parse_args())
	acl_files = args["<acl>"]
	print_accesses = args["print_acls"]
	print_clang = args["print_clang"]
	print_verbose = args["print_verbose"]

	if not print_accesses and \
	   not print_clang:
		print_accesses = True

def main():
	check_args(sys.argv)
	load_acls()
	print_acls()
	print_code()

if __name__ == "__main__":
	main()
