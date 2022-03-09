#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

import sys
import re
import argparse
import os
import ctypes

ACCESS_SIZE = 4

FUNCTION_PREFIX_BLACKLIST = ("llvm.", "__cxx_global_var_init")

ADD_ADDR_OFFSET = {
	# stm32_usbreset
	40569 : [ 2312+(0<<5), 2312+(1<<5), 2312+(2<<5), 2312+(3<<5) ], 
	40575 : [ 2824+(0<<5), 2824+(1<<5), 2824+(2<<5), 2824+(3<<5) ], 
	# stm32_epin_configure
	41211 : [ 2304+((0x80|1)<<5), 2304+(3<<5), 2304+((0x80|3)<<5), 2304+(2<<5) ], 
	# stm32_epout_configure
	41384 : [ 2304+((0x80|1)<<5), 2304+(3<<5), 2304+((0x80|3)<<5), 2304+(2<<5) ],
	# stm32_ep_clrstall
	42651 : [ 2304+((0x80|1)<<5), 2304+(3<<5), 2304+((0x80|3)<<5), 2304+(2<<5) ], 
	42664 : [ 2816+((0x80|1)<<5), 2816+(3<<5), 2816+((0x80|3)<<5), 2816+(2<<5) ], 
	# stm32_epin_setstall
	42764 : [ 2304+((0x80|1)<<5), 2304+(3<<5), 2304+((0x80|3)<<5), 2304+(2<<5) ], 
	# stm32_epout_setstall
	42803 : [ 2816+((0x80|1)<<5), 2816+(3<<5), 2816+((0x80|3)<<5), 2816+(2<<5) ], 
	# stm32_epin_request
	42966 : [ 2304+((0x80|1)<<5), 2304+(3<<5), 2304+((0x80|3)<<5), 2304+(2<<5) ], 
	43114 : [ 2328+((0x80|1)<<5), 2328+(3<<5), 2328+((0x80|3)<<5), 2328+(2<<5) ], 
	# stm32_epout_request
	43269 : [ 2816+((0x80|1)<<5), 2816+(3<<5), 2816+((0x80|3)<<5), 2816+(2<<5) ], 
	43343 : [ 2832+((0x80|1)<<5), 2832+(3<<5), 2832+((0x80|3)<<5), 2832+(2<<5) ], 
	43372 : [ 2816+((0x80|1)<<5), 2816+(3<<5), 2816+((0x80|3)<<5), 2816+(2<<5) ], 
	# stm32_epin_transfer
	43436 : [ 2320+((0x80|1)<<5), 2320+(3<<5), 2320+((0x80|3)<<5), 2320+(2<<5) ], 
	43504 : [ 2320+((0x80|1)<<5), 2320+(3<<5), 2320+((0x80|3)<<5), 2320+(2<<5) ], 
	43513 : [ 2304+((0x80|1)<<5), 2304+(3<<5), 2304+((0x80|3)<<5), 2304+(2<<5) ], 
	43563 : [ 2304+((0x80|1)<<5), 2304+(3<<5), 2304+((0x80|3)<<5), 2304+(2<<5) ], 
	# stm32_txfifo_write
	43602 : [ 4096+((0x80|1)<<12), 4096+(3<<12), 4096+((0x80|3)<<12), 4096+(2<<12) ], 
	# stm32_epin_disable
	43680 : [ 2304+((0x80|1)<<5), 2304+(3<<5), 2304+((0x80|3)<<5), 2304+(2<<5) ], 
	43703 : [ 2304+((0x80|1)<<5), 2304+(3<<5), 2304+((0x80|3)<<5), 2304+(2<<5) ], 
	43725 : [ 2312+((0x80|1)<<5), 2312+(3<<5), 2312+((0x80|3)<<5), 2312+(2<<5) ], 
	# stm32_epout_disable
	43796 : [ 2816+((0x80|1)<<5), 2816+(3<<5), 2816+((0x80|3)<<5), 2816+(2<<5) ], 
	43819 : [ 2824+((0x80|1)<<5), 2824+(3<<5), 2824+((0x80|3)<<5), 2824+(2<<5) ], 
	# stm32_configgpio
	48964 : [ 8+((((3<<10)|(1<<9)|(9))&15)&12), 
			  8+((((1<<18)|(1<<9)|(2<<10)|(4<<4)|(12))&15)&12),
			  8+((((3<<18)|(2))&15)&12),
			  8+((((3<<18)|(3))&15)&12),
			  8+((((3<<18)|(2<<4)|(3))&15)&12),
			  8+((((3<<18)|(2<<4)|(4))&15)&12),
			  8+((((3<<18)|(2<<4)|(5))&15)&12),
			  8+((((3<<18)|(1<<10)|(8))&15)&12),
			  8+((((3<<18)|(1<<10)|(1<<7)|(3))&15)&12),
			  8+((((1<<16)|(1<<4)|(5))&15)&12),
			  8+((((1<<16)|(4<<4)|(10))&15)&12),
			  8+((((1<<16)|(4<<4)|(15))&15)&12),
			  8+((((3<<18)|(1<<10)|(3<<4)|(14))&15)&12),
			  8+((((3<<18)|(2<<10)|(1<<7)|(4<<4)|(4))&15)&12),
			  8+((((3<<18)|(2<<10)|(1<<7)|(2<<4)|(14))&15)&12),
			  8+((((3<<18)|(2<<10)|(1<<7)|(2<<4)|(15))&15)&12),
			  8+((((3<<18)|(2<<10)|(1<<7)|(2<<4)|(13))&15)&12),
			  8+((((3<<18)|(1<<10)|(1<<7)|(2<<4)|(13))&15)&12),
			  8+((((3<<18)|(1<<10)|(1<<7)|(4<<4)|(2))&15)&12),
			  8+((((3<<18)|(1<<10)|(1<<7)|(1<<4)|(0))&15)&12),
			  8+((((3<<18)|(1<<10)|(1<<7)|(1<<4)|(0))&15)&12),
			  8+((((1<<10)|(3<<4)|(0))&15)&12),
			  8+((((1<<10)|(1<<4)|(1))&15)&12),
			  8+((((1<<10)|(1<<4)|(1))&15)&12),
			  8+((((1<<10)|(3<<4)|(15))&15)&12),
			  8+((((3<<18)|(1<<10)|(1<<8)|(3<<4)|(10))&15)&12),
			  8+((((2<<16)|(2<<4)|(13))&15)&12),
			  8+((((2<<16)|(2<<4)|(15))&15)&12),
			  8+((((2<<16)|(3<<4)|(7))&15)&12),
			  8+((((2<<16)|(2<<4)|(2))&15)&12),
			  8+((((2<<16)|(5))&15)&12),
			  8+((((2<<16)|(6))&15)&12),
			  8+((((2<<16)|(7))&15)&12),
			  8+((((2<<16)|(1<<4)|(0))&15)&12),
			  8+((((2<<16)|(1<<4)|(1))&15)&12),
			  8+((((2<<16)|(1<<4)|(4))&15)&12),
			  8+((((1<<16)|(1<<8)|(3<<4)|(15))&15)&12),
			  8+((((3<<18)|(1<<8)|(4<<4)|(3))&15)&12),
			  8+((((3<<18)|(1<<8)|(2<<4)|(13))&15)&12),
			  8+((((3<<18)|(1<<8)|(2<<4)|(15))&15)&12),
			  8+((((3<<18)|(1<<8)|(3<<4)|(7))&15)&12),
			  8+((((3<<18)|(1<<8)|(2<<4)|(2))&15)&12),
			  8+((((3<<18)|(1<<8)|(0<<4)|(8))&15)&12),
			  8+((((9<<12)|(2<<10)|(3<<4)|(0))&15)&12),
			  8+((((9<<12)|(2<<10)|(3<<4)|(1))&15)&12),
			  8+((((9<<12)|(1<<16)|(2<<10)|(1<<4)|(12))&15)&12),
			  8+((((9<<12)|(2<<10)|(1<<4)|(6))&15)&12),
			  8+((((2<<12)|(2<<10)|(3<<4)|(13))&15)&12),
			  8+((((3<<18)|(1<<8)|(2<<7)|(5))&15)&12),
			  8+((((3<<18)|(0<<4)|(15))&15)&12),
			  8+(((15)&15)&12),
			  8+((((1<<12)|(2<<10)|(4<<4)|(9))&15)&12),
			  8+((((1<<12)|(2<<10)|(4<<4)|(11))&15)&12),
			  8+((((1<<12)|(2<<10)|(4<<4)|(13))&15)&12),
			  8+((((1<<12)|(2<<10)|(4<<4)|(14))&15)&12),
			  8+((((1<<12)|(2<<10)|(3<<4)|(13))&15)&12),
			  8+((((1<<12)|(1<<16)|(0<<4)|(10))&15)&12)	],
	# stm32_pwr_modifyreg
	110303 : [ 0, 4 ], 
	# up_usbuninitialize
	116189 : [ 2312+(0<<5), 2312+(1<<5), 2312+(2<<5), 2312+(3<<5) ], 
	116195 : [ 2824+(0<<5), 2824+(1<<5), 2824+(2<<5), 2824+(3<<5) ], 
	# stm32_hwinitialize
	116690: [ 2304+(0<<5), 2304+(1<<5), 2304+(2<<5), 2304+(3<<5) ], 
	116712: [ 2304+(0<<5), 2304+(1<<5), 2304+(2<<5), 2304+(3<<5) ], 
	116718 : [ 2320+(0<<5), 2320+(1<<5), 2304+(2<<5), 2320+(3<<5) ], 
	116724 : [ 2312+(0<<5), 2312+(1<<5), 2312+(2<<5), 2312+(3<<5) ], 
	116748 : [ 2816+(0<<5), 2816+(1<<5), 2816+(2<<5), 2816+(3<<5) ], 
	116770 : [ 2816+(0<<5), 2816+(1<<5), 2816+(2<<5), 2816+(3<<5) ], 
	116776 : [ 2832+(0<<5), 2832+(1<<5), 2832+(2<<5), 2832+(3<<5) ], 
	116782 : [ 2824+(0<<5), 2824+(1<<5), 2824+(2<<5), 2824+(3<<5) ], 
	# stm32_epout_interrupt
	116856 : [ 2824+(0<<5), 2824+(1<<5), 2824+(2<<5), 2824+(3<<5) ], 
	116863 : [ 2824+(0<<5), 2824+(1<<5), 2824+(2<<5), 2824+(3<<5) ], 
	116899 : [ 2824+(0<<5), 2824+(1<<5), 2824+(2<<5), 2824+(3<<5) ], 
	116916 : [ 2824+(0<<5), 2824+(1<<5), 2824+(2<<5), 2824+(3<<5) ], 
	116935 : [ 2824+(0<<5), 2824+(1<<5), 2824+(2<<5), 2824+(3<<5) ], 
	116966 : [ 2824+(0<<5), 2824+(1<<5), 2824+(2<<5), 2824+(3<<5) ], 
	# stm32_epin_interrupt
	117039 : [ 2312+(0<<5), 2312+(1<<5), 2312+(2<<5), 2312+(3<<5) ], 
	117093 : [ 2312+(0<<5), 2312+(1<<5), 2312+(2<<5), 2312+(3<<5) ], 
	117116 : [ 2312+(0<<5), 2312+(1<<5), 2312+(2<<5), 2312+(3<<5) ], 
	117135 : [ 2312+(0<<5), 2312+(1<<5), 2312+(2<<5), 2312+(3<<5) ], 
	117156 : [ 2312+(0<<5), 2312+(1<<5), 2312+(2<<5), 2312+(3<<5) ], 
	117191 : [ 2312+(0<<5), 2312+(1<<5), 2312+(2<<5), 2312+(3<<5) ] }

GPIO_BASE_ADDRS= [ 0x40020000, 0x40020400, 0x40020800, 0X40020C00, 0x40021000, 0x40021400, 0x40021800, 0x40021C00, 0x40022000 ]

bitcode = None
ranges = None

int_as_addr = None

print_accesses = None
print_verbose = None

functions = list()
current_function = None

class Function:
	def __init__(self, name):
		self.name = name
		self.accesses = list()

	def __str__(self):
		return self.name

class StoreAddrToVar:
	def __init__(self, addr, var):
		self.addr = addr
		self.var = var

	def __str__(self):
		return "%08x-%08x %-2s # %%%s = 0x%08x [%d]" % \
			   (self.addr, self.addr+ACCESS_SIZE, "rw", 
				self.var, self.addr, ACCESS_SIZE)

class StoreValToAddr:
	def __init__(self, val, addr):
		self.val = val
		self.addr = addr

	def __str__(self):
		return "%08x-%08x %-2s # *0x%08x = 0x%08x [%d]" % \
			   (self.addr, self.addr+ACCESS_SIZE, "rw", 
				self.addr, self.val, ACCESS_SIZE)

class StoreVarToAddr:
	def __init__(self, var, addr):
		self.var = var
		self.addr = addr

	def __str__(self):
		return "%08x-%08x %-2s # *0x%08x = %%%s [%d]" % \
			   (self.addr, self.addr+ACCESS_SIZE, "rw", 
				self.addr, self.var, ACCESS_SIZE)

class LoadAddrToVar:
	def __init__(self, addr, var):
		self.addr = addr
		self.var = var

	def __str__(self):
		return "%08x-%08x %-2s # %%%s = 0x%08x [%d]" % \
			   (self.addr, self.addr+ACCESS_SIZE, "r", 
				self.var, self.addr, ACCESS_SIZE)

class AddAddrToVar:
	def __init__(self, addr, src, off, dest):
		self.addr = addr
		self.src = src
		self.off = off
		self.dest = dest

	def __str__(self):
		return "%08x-%08x %-2s # %%%s = 0x%08x + 0x%x [%d]" % \
			   (self.addr, self.addr+ACCESS_SIZE, "rw", 
				self.dest, self.src, self.off, ACCESS_SIZE)

def check_ranges(addr):
	for rng in ranges:
		if rng[0] <= addr and addr <= rng[1]:
			return True
	return False

def function_open(line, lineno):
	global current_function

	m = re.match(r"define [^@]+ @([^(]+)\(.*\).*{", line)
	if m is None:
		return False
	name = m.group(1)

	# Ignore cloned functions
	if re.match(r".+\.\d+$", name) is not None:
		return True

	for prefix in FUNCTION_PREFIX_BLACKLIST:
		if name.startswith(prefix):
			return True

	current_function = Function(name)

	functions.append(current_function)

	if print_accesses:
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

	if print_accesses:
		if print_verbose:
			print("")

	return True

def store_addr_to_var(line, lineno):
	if int_as_addr:
		m = re.match(r"  store( volatile)? (i32\*+ inttoptr \()?i32 (-?\d+)( to i32\*\))?, i32\*+ %([^,]+), align \d+.*", line)
		if m is None:
			return False
		addr = ctypes.c_uint(int(m.group(3))).value
		var = m.group(5)
	else:
		m = re.match(r"  store( volatile)? i32\*+ inttoptr \(i32 (-?\d+) to i32\*\), i32\*+ %([^,]+), align \d+.*", line)
		if m is None:
			return False
		addr = ctypes.c_uint(int(m.group(2))).value
		var = m.group(3)

	if current_function is None:
		return True

	if not check_ranges(addr):
		return True

	access = StoreAddrToVar(addr, var)

	current_function.accesses.append(access)

	if print_accesses:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
			print("  " + str(access))

	return True

def store_val_to_addr(line, lineno):
	m = re.match(r"  store( volatile)? i32 (-?\d+), i32\*+ inttoptr \(i32 (-?\d+) to i32\*+\), align \d+.*", line)
	if m is None:
		return False
	val = ctypes.c_uint(int(m.group(2))).value
	addr = ctypes.c_uint(int(m.group(3))).value

	if current_function is None:
		return True

	if not check_ranges(addr):
		return True

	access = StoreValToAddr(val, addr)

	current_function.accesses.append(access)

	if print_accesses:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
			print("  " + str(access))

	return True

def store_var_to_addr(line, lineno):
	m = re.match(r"  store( volatile)? i32 %([^,]+), i32\*+ inttoptr \(i32 (-?\d+) to i32\*+\), align \d+.*", line)
	if m is None:
		return False
	var = m.group(2)
	addr = ctypes.c_uint(int(m.group(3))).value

	if current_function is None:
		return True

	if not check_ranges(addr):
		return True

	access = StoreVarToAddr(var, addr)

	current_function.accesses.append(access)

	if print_accesses:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
			print("  " + str(access))

	return True

def load_addr_to_var(line, lineno):
	if int_as_addr:	
		m = re.match(r"  %([^ ]+) = load( volatile)?( i32,)? (i32\*+ inttoptr \()?i32 (-?\d+)( to i32\*\))?, align \d+.*", line)
		if m is None:
			return False
		var = m.group(1)
		addr = ctypes.c_uint(int(m.group(5))).value
	else:
		m = re.match(r"  %([^ ]+) = load( volatile)?( i32,)? i32\*+ inttoptr \(i32 (-?\d+) to i32\*\), align \d+.*", line)
		if m is None:
			return False
		var = m.group(1)
		addr = ctypes.c_uint(int(m.group(4))).value

	if current_function is None:
		return True

	if not check_ranges(addr):
		return True

	access = LoadAddrToVar(addr, var)

	current_function.accesses.append(access)

	if print_accesses:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
			print("  " + str(access))

	return True

def add_addr_to_var(line, lineno):
	m = re.match(r"  %([^ ]+) = add( nuw nsw| nuw| nsw)? i32 (\d+), %(\d+).*", 
				 line)
	if m is None:
		return False
	res = m.group(1)
	addr = ctypes.c_uint(int(m.group(3))).value
	op = m.group(4)

	if current_function is None:
		return True

	if not check_ranges(addr):
		return True

	if lineno in ADD_ADDR_OFFSET:
		for off in ADD_ADDR_OFFSET[lineno]:
			access = AddAddrToVar(addr+off, addr, off, res)
			current_function.accesses.append(access)
		if lineno == 48964:
			for base in GPIO_BASE_ADDRS:
				access = AddAddrToVar(base+0, base, 0, res)
				current_function.accesses.append(access)
				access = AddAddrToVar(base+4, base, 4, res)
				current_function.accesses.append(access)
	else:
		access = AddAddrToVar(addr+0, addr, 0, res)
		current_function.accesses.append(access)

	if print_accesses:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
			print("  " + str(access))

	return True

def print_memory_accesses():
	if not print_accesses:
		return
	for function in functions:
		if len(function.accesses) == 0:
			continue
		print("Function: '" + str(function) + "'")
		for access in function.accesses:
			print("  " + str(access))
		print("")

def range_type(x):
	m = re.match(r"(0x)?([0-9a-fA-F]+)-(0x)?([0-9a-fA-F]+)", x)
	if m is None:
		raise ValueError()
	start = int(m.group(2), 16)
	end = int(m.group(4), 16)
	return (start, end)

def check_args(argv):
	global bitcode, ranges
	global int_as_addr
	global print_accesses
	global print_verbose

	desc = "Find accesses to a specified MMIO memory region"
	parser = argparse.ArgumentParser(description=desc)
	parser.add_argument("<bitcode>", type=argparse.FileType("r"),  
						help="LLVM bitcode disassembled by 'llvm-dis'")
	parser.add_argument("<start-end>", type=range_type,  nargs="+", 
						help="Address range of a target memory region")
	parser.add_argument("-a", "--int-as-addr", dest="int_as_addr", 
						action="store_true", 
						help="Treat all 32-bit integers as memory addresses")
	parser.add_argument("-c", "--print-accesses", dest="print_accesses", 
						action="store_true", 
						help="Print memory accesses (default: enabled)")
	parser.add_argument("-v", "--print-verbose", dest="print_verbose", 
						action="store_true", 
						help="Print verbose messages")

	parser.set_defaults(int_as_addr=False)
	parser.set_defaults(print_accesses=False)
	parser.set_defaults(print_verbose=False)

	args = vars(parser.parse_args())
	bitcode = args["<bitcode>"]
	ranges = args["<start-end>"]
	int_as_addr = args["int_as_addr"]
	print_accesses = args["print_accesses"]
	print_verbose = args["print_verbose"]

	if not print_accesses:
		print_accesses = True

def main():
	check_args(sys.argv)

	for i, line in enumerate(bitcode):
		line = line.rstrip()
		if function_open(line, i+1):
			continue
		if function_close(line, i+1):
			continue
		if store_addr_to_var(line, i+1):
			continue
		if store_val_to_addr(line, i+1):
			continue
		if store_var_to_addr(line, i+1):
			continue
		if load_addr_to_var(line, i+1):
			continue
		if add_addr_to_var(line, i+1):
			continue

	print_memory_accesses()

if __name__ == "__main__":
	main()
