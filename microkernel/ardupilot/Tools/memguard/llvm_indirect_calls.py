#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

import sys
import re
import argparse
import os
import itertools
from callgraph import demangle

FUNCTION_PREFIX_BLACKLIST = ("llvm.", "__cxx_global_var_init")

FUNCTION_ARG_BLACKLIST = (
	"readnone", 
	"readonly", 
	"nocapture", 
	"noalias", 
	"returned", 
	"sret", 
	"arm_aapcs_vfpcc", 
	"zeroext", 
	"signext", 
	"weak_odr", 
	"weak", 
	"hidden", 
	"internal")

BASIC_ARG_TYPES = {"", "void", "i1", "i8", "i16", "i32", "float", "double", 
				   "void*", "i1*", "i8*", "i16*", "i32*", "i8**"}

MAX_MATCHING_FUNCTIONS = 20

bitcode = None
layouts = None

print_calls = None
print_unsound_calls = None
print_functions = None
print_classes = None
print_vtables = None
print_verbose = None

function_names = dict() # function name: function
function_types = dict() # function type: functions
current_function = None

class_names = dict() # class name: class
current_class = None

class Function:
	def __init__(self, name, args, ret):
		self.name = name
		self.args = args
		self.ret = ret
		self.unsound_calls = dict() # bitcode line: function types

	def __str__(self):
		return self.ret + " " + self.name + " " + str(self.args)

class Class:
	def __init__(self, name):
		self.name = name
		self.parents = dict()
		self.children = dict()
	
	def __str__(self):
		return self.name

def split(s):
	l = list()
	depth = 0
	begin = 0
	for i in range(len(s)):
		c = s[i]
		if c == '(' or c == '<':
			depth += 1
		elif c == ')' or c == '>':
			depth -= 1
		elif c == ',':
			if depth == 0:
				l.append(s[begin:i])
				begin = i+2
	l.append(s[begin:])
	return l

def data_type(t):
	if len(t) == 0:
		return t
	# Remove noisy keywords
	for keyword in FUNCTION_ARG_BLACKLIST:
		pos = t.find(keyword + " ")
		if pos != -1:
			t = t[:pos] + t[pos+len(keyword)+1:]
			continue
		pos = t.rfind(" " + keyword)
		if pos != -1:
			t = t[:pos] + t[pos+len(keyword)+1:]
	# Remove dereferenceable(*) keywords
	m = re.search(r"( dereferenceable\(\d+\))", t)
	if m is not None:
		span = m.span(1)
		t = t[:span[0]] + t[span[1]:]
	else:
		m = re.search(r"(dereferenceable\(\d+\) )", t)
		if m is not None:
			span = m.span(1)
			t = t[:span[0]] + t[span[1]:]
	# Remove template specifiers
	beg = t.find("<")
	if beg != -1:
		end = t.rfind(">")
		assert(end != -1)
		t = t[:beg] + t[end+1:]
	# Remove double quotes
	while True:
		beg = t.find("\"")
		if beg == -1:
			break
		end = t.rfind("\"")
		assert(end != -1)
		t = t[:beg] + t[beg+1:end] + t[end+1:]
	# Remove '%' symbols
	while True:
		pos = t.find("%")
		if pos == -1:
			break
		t = t[:pos] + t[pos+1:]
	# Resolve type name conflicts
	while True:
		m = re.match(r"(class|struct|union)\..+(\.\d+|\.base).*", t)
		if m is None:
			break
		span = m.span(2)
		t = t[:span[0]] + t[span[1]:]
	return t

def function_type(args, ret):
	return ret + " " + str(args)

def match_function_type(args, ret):
	tbl = list()

	for t in [ret]+args:
		l = list()
		tbl.append(l)

		# Original type
		l.append(t)

		# Class pointer?
		m = re.match(r"(class|struct|union)\.([^\*]+)\*", t)
		if m is None:
			continue

		name = m.group(1) + "." + m.group(2)
		prefix = t[:m.span(1)[0]]
		suffix = t[m.span(2)[1]:]

		assert(name in class_names)
		c = class_names[name]

		# Child class pointers
		stack = list()
		for child in c.children.itervalues():
			stack.append(child)
		while len(stack) > 0:
			cur = stack.pop()
			l.append(prefix + cur.name + suffix)
			for child in cur.children.itervalues():
				stack.append(child)

		# Parent class pointers
		stack = list()
		for parent in c.parents.itervalues():
			stack.append(parent)
		while len(stack) > 0:
			cur = stack.pop()
			l.append(prefix + cur.name + suffix)
			for parent in cur.parents.itervalues():
				stack.append(parent)

	combs = list(itertools.product(*tbl))
	cands = list()
	for comb in combs:
		cands.append(function_type(list(comb[1:]), comb[0]))

	types = list()
	for cand in cands:
		matched = cand in function_types
		if matched:
			types.append(cand)
		if print_calls:
			if print_verbose:
				print("#     " + ("O" if matched else "X") + ": " + cand)

	return types

def function_declare(line, lineno):
	m = re.match(r"declare ([^@]+) @([^(]+)\((.*)\).*", line)
	if m is None:
		return False
	ret = m.group(1)
	name = m.group(2)
	args = split(m.group(3))

	# Ignore cloned functions
	if re.match(r".+\.\d+$", name) is not None:
		return True

	for prefix in FUNCTION_PREFIX_BLACKLIST:
		if name.startswith(prefix):
			return True

	ret = data_type(ret)
	for i in range(len(args)):
		args[i] = data_type(args[i])
		
	f = Function(name, args, ret)

	assert(name not in function_names)
	function_names[name] = f

	t = function_type(f.args, f.ret)
	if t not in function_types:
		function_types[t] = list()
	function_types[t].append(f)

	if print_functions or print_calls:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
		if print_functions:
			print(str(f))
		elif print_calls:
			print("Call graph node for function: '" + f.name + 
				  "'<<" + ("0x%x" % 0) + ">>  #uses=" + str(0))
			print("")

	return True

def function_define(line, lineno):
	m = re.match(r"define ([^@]+) @([^(]+)\((.*)\).*{", line)
	if m is None:
		return False
	ret = m.group(1)
	name = m.group(2)
	args = split(m.group(3))

	# Ignore cloned functions
	if re.match(r".+\.\d+$", name) is not None:
		return True

	for prefix in FUNCTION_PREFIX_BLACKLIST:
		if name.startswith(prefix):
			return True

	# Leave only the types of the arguments
	for i in range(len(args)):
		a = re.match(r"(.+) %.+", args[i])
		if a is not None:
			args[i] = a.group(1)
	
	ret = data_type(ret)
	for i in range(len(args)):
		args[i] = data_type(args[i])
		
	f = Function(name, args, ret)

	assert(name not in function_names)
	function_names[name] = f

	t = function_type(f.args, f.ret)
	if t not in function_types:
		function_types[t] = list()
	function_types[t].append(f)
	
	if print_functions:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
		print(str(f))

	return True

def class_define(line, lineno):
	m = re.match(r"%\"?(class|struct|union)\.([^\"]+)\"? = type \{ (.+) \}", 
				 line)
	if m is None:
		return False

	name = data_type(m.group(1) + "." + m.group(2))
	items = split(m.group(3))

	if name not in class_names:
		class_names[name] = Class(name)
	c = class_names[name]

	b = re.match(r"%\"?(class|struct|union)\.([^\"*]+)\"?", items[0])
	if b is None:
		return True

	base_name = data_type(b.group(1) + "." + b.group(2))

	# Self inheritance?
	if base_name == name:
		return True

	# Is base class a descendant?
	stack = list()
	for child in c.children.itervalues():
		stack.append(child)
	while len(stack) > 0:
		cur = stack.pop()
		if base_name == cur.name:
			return True
		for child in cur.children.itervalues():
			stack.append(child)

	if base_name not in class_names:
		class_names[base_name] = Class(base_name)
	base = class_names[base_name]

	if c.name not in base.children:
		base.children[c.name] = c

	if base.name not in c.parents:
		c.parents[base.name] = base

	if print_classes:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
			print(str(c) + " : " + str(base))

	return True
	
def vtable_define(line, lineno):
	m = re.match(r"@(_ZTVN.+)", line)
	if m is None:
		return False
	
	demangled = demangle(m.group(1))

	m = re.match(r"vtable for (.+) = .+ \[.+\] \[(.+)\](, comdat)?, align \d+", demangled)
	if m is None:
		print("Error: " + line)
		print("Error: " + demangled)
	assert(m is not None)
	name = m.group(1)
	items = split(m.group(2))

	if print_vtables:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
		print("vtable for " + name)
		for item in items:
			print("  " + item)

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

	for prefix in FUNCTION_PREFIX_BLACKLIST:
		if name.startswith(prefix):
			return True

	current_function = function_names[name]

	if print_calls:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
			print("# " + str(current_function))
		print("Call graph node for function: '" + 
			  current_function.name + 
			  "'<<" + ("0x%x" % 0) + ">>  #uses=" + str(0))

	return True

def function_close(line, lineno):
	global current_function

	if line != "}":
		return False
	
	if current_function is None:
		return True

	current_function = None

	if print_calls:
		print("")

	return True

def indirect_call(line, lineno):
	if not " call " in line:
		return False

	if " call void @llvm.dbg.value" in line:
		return False

	if " call void @llvm.dbg.declare" in line:
		return False

	if " @llvm.invariant.start" in line:
		return False

	m = re.search(" call arm_aapcs_vfpcc (.*) @", line)
	if m != None:
		return False

	m = re.search(r" call .* @", line)
	if m != None:
		return False

	if " sideeffect " in line:
		return False

	m = re.search(r" call ([^@]+) %(\d+)\((.*)\).*", line)
	if m is None:
		return False
	
	ret = m.group(1)
	name = m.group(2)
	args = None

	if current_function is None:
		return True

	# Functions are sometimes casted before invocation
	r = re.match(r"(.+) \((.*)\)\*", ret)
	if r is None:
		args = split(m.group(3))
	else:
		ret = r.group(1)
		args = split(r.group(2))
	
	# Leave only the types of the arguments
	for i in range(len(args)):
		arg = args[i]
		pos = arg.rfind(" ")
		if pos != -1:
			args[i] = arg[:pos]
	
	ret = data_type(ret)
	for i in range(len(args)):
		args[i] = data_type(args[i])

	if print_calls:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
			print("#   indirect call " + ret + " %" + name + " " + str(args))

	# Find all matching function types
	types = match_function_type(args, ret)

	# No matching function type?
	if len(types) == 0:
		if print_calls:
			print("# Warning: no matching function: " + 
				  function_type(args, ret))
			sys.stderr.write("Warning: no matching function: " + 
							 function_type(args, ret) + os.linesep)
			sys.stderr.write("# " + str(lineno) + ": " + line + os.linesep)
			sys.stderr.write(os.linesep)
		return True

	for t in types:
		# Assume no recursive indirect call
		targets = [f for f in function_types[t] if f != current_function]

		# Too many matching functions?
		if len(targets) > MAX_MATCHING_FUNCTIONS:
			if lineno not in current_function.unsound_calls:
				current_function.unsound_calls[lineno] = list()
			current_function.unsound_calls[lineno].append(t)
			if print_calls:
				print("# Warning: too many matching functions: " + t + " (" + 
					  str(len(targets)) + ")")
				sys.stderr.write("Warning: too many matching functions: " + 
								 t + " (" + str(len(targets)) + ")" + 
								 os.linesep)
				sys.stderr.write("# " + str(lineno) + ": " + line + os.linesep)
				sys.stderr.write(os.linesep)
			continue

		for target in targets:
			if print_calls:
				if print_verbose:
					print("#     " + str(target))
				print("  CS<" + ("0x%x" % 0) + "> calls function '" + 
					  target.name + "'")

	return True

def class_open(line, lineno):
	global current_class

	m = re.match(r"[ ]+0 \| (class|struct|union) (.+)", line)
	if m is None:
		return False

	t = m.group(2)

	# Remove ending phases between parantheses
	while t[-1] == ')':
		pos = t.rfind("(")
		assert(pos != -1)
		t = t[:pos].rstrip()
		if len(t) == 0: # class.(lambda at <codeloc>)
			return True

	name = data_type(m.group(1) + "." + t)

	if name not in class_names:
		class_names[name] = Class(name)
	current_class = class_names[name]

	if print_classes:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
			print(str(current_class))

	return True

def class_close(line, lineno):
	global current_class

	if line != "":
		return False

	if current_class is None:
		return True

	current_class = None
	
	return True

def class_base(line, lineno):
	m = re.match(r"[ ]+0 \|   (class|struct|union) (.+) \((primary )?base\).*",
				 line)
	if m is None:
		return False

	name = data_type(m.group(1) + "." + m.group(2))

	assert(name in class_names)
	base = class_names[name]

	assert(current_class is not None)
	if current_class.name not in base.children:
		base.children[current_class.name] = current_class

	if base.name not in current_class.parents:
		current_class.parents[base.name] = base

	if print_classes:
		if print_verbose:
			print("# " + str(lineno) + ": " + line)
			print("  " + ": " + str(base))
	
	return True

def print_class_hierarchy():
	if not print_classes:
		return
	for n, c in sorted(class_names.iteritems()):	
		print(str(c))
		for m, child in sorted(c.children.iteritems()):
			print("  " + str(child))

def print_unsound_call_info():
	if not print_unsound_calls:
		return
	"""
	for n, f in sorted(function_names.iteritems()):
		if len(f.unsound_calls) == 0:
			continue
		print(f.name + ":")
		for lineno, types in sorted(f.unsound_calls.iteritems()):
			print("  " + str(lineno) + ": indirect call " + types[0] + ": " + 
				  str(len(types)) + " function types")
			for t in types:
				targets = function_types[t]
				print("    " + t + ": " + str(len(targets)) + 
					  " matching functions")
				if print_verbose:
					names = [target.name for target in targets]
					for name in sorted(names):
						print("      " + name)
		print("")
	"""
	unsound_calls = dict() # function type: source lines
	source_lines = dict() # source line: containing function
	for n, f in sorted(function_names.iteritems()):
		if len(f.unsound_calls) == 0:
			continue
		for lineno, types in f.unsound_calls.iteritems():
			source_lines[lineno] = f
			for t in types:
				if t not in unsound_calls:
					unsound_calls[t] = list()
				unsound_calls[t].append(lineno)
	print("# " + str(len(unsound_calls)) + " indirect call targets"
		  " (each yields more than " + str(MAX_MATCHING_FUNCTIONS) + 
		  " matching functions)")
	print("")
	for t, linenos in sorted(unsound_calls.iteritems()):
		targets = function_types[t]
		t = t.replace("''", "void")
		t = t.replace("'", "")
		t = t.replace("[", "(")
		t = t.replace("]", ")")
		print("indirect call %? " + t + ": ")
		print("  # " + str(len(linenos)) + " indirect call locations"
			  " (" + bitcode.name  + ")")
		for lineno in sorted(linenos):
			print("  " + str(lineno) + 
				  " (" + source_lines[lineno].name + ")")
		print("  # " + str(len(targets)) + " matching functions")
		names = [target.name for target in targets]
		for name in sorted(names):
			print("  " + name)
		print("")

def check_args(argv):
	global bitcode, layouts
	global print_calls, print_unsound_calls
	global print_functions, print_vtables, print_classes
	global print_verbose

	desc = "Find indirect call targets through type-based function matching"
	parser = argparse.ArgumentParser(description=desc)
	parser.add_argument("<bitcode>", type=argparse.FileType("r"),  
						help="LLVM bitcode disassembled by 'llvm-dis'")
	parser.add_argument("<layouts>", type=argparse.FileType("r"),  
						help="Record layout information dumped by "
							 "'clang -cc1 -fdump-record-layouts'")
	parser.add_argument("-c", "--print-calls", dest="print_calls", 
						action="store_true", 
						help="Print indirect calls (default: enabled)")
	parser.add_argument("-d", "--unsound-calls", dest="print_unsound_calls", 
						action="store_true", 
						help="Print likely unsound indirect calls")
	parser.add_argument("-f", "--print-functions", dest="print_functions", 
						action="store_true", 
						help="Print function types")
	parser.add_argument("-l", "--print-classes", dest="print_classes", 
						action="store_true", 
						help="Print class hierarchies")
	parser.add_argument("-t", "--print-vtables", dest="print_vtables", 
						action="store_true", 
						help="Print virtual method tables")
	parser.add_argument("-v", "--print-verbose", dest="print_verbose", 
						action="store_true", 
						help="Print verbose messages")

	parser.set_defaults(print_calls=False)
	parser.set_defaults(print_unsound_calls=False)
	parser.set_defaults(print_functions=False)
	parser.set_defaults(print_vtables=False)
	parser.set_defaults(print_classes=False)
	parser.set_defaults(print_verbose=False)

	args = vars(parser.parse_args())
	bitcode = args["<bitcode>"]
	layouts = args["<layouts>"]
	print_calls = args["print_calls"]
	print_unsound_calls = args["print_unsound_calls"]
	print_functions = args["print_functions"]
	print_vtables = args["print_vtables"]
	print_classes = args["print_classes"]
	print_verbose = args["print_verbose"]

	if not print_calls and \
	   not print_unsound_calls and \
	   not print_functions and \
	   not print_classes and \
	   not print_vtables:
		print_calls = True

def main():
	check_args(sys.argv)

	for i, line in enumerate(layouts):
		line = line.rstrip()
		if class_open(line, i+1):
			continue
		if class_close(line, i+1):
			continue
		if class_base(line, i+1):
			continue
	
	for i, line in enumerate(bitcode):
		line = line.rstrip()
		if function_declare(line, i+1):
			continue
		if function_define(line, i+1):
			continue
		if class_define(line, i+1):
			continue
		if vtable_define(line, i+1):
			continue

	bitcode.seek(0)
	for i, line in enumerate(bitcode):
		line = line.rstrip()
		if function_open(line, i+1):
			continue
		if function_close(line, i+1):
			continue
		if indirect_call(line, i+1):
			continue

	print_class_hierarchy()
	print_unsound_call_info()

if __name__ == "__main__":
	main()
