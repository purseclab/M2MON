#!/usr/bin/env python
# author: Chung Hwan Kim <chungkim@cs.purdue.edu>

import sys
import argparse
import os
from collections import deque
import callgraph as c

blacklist = set([
	# start functions
	"os_start", 
	"nsh_consolemain", 
	"sercon_main", 
	"serdis_main", 
	"work_hpthread", 
	"work_lpthread", 
	"nsh_main", 
	"_ZL9main_loopiPPc", 
	"ArduPilot_main", 
	"batt_smbus_main", 
	"ets_airspeed_main", 
	"hmc5883_main", 
	"l3gd20_main", 
	"ll40ls_main",
	"lsm303d_main",
	"mb12xx_main",
	"meas_airspeed_main",
	"mkblctrl_main",
	"mpu6000_main",
	"mpu9250_main",
	"ms5611_main",
	"oreoled_main",
	"pwm_input_main",
	"px4flow_main",
	"fmu_main",
	"px4io_main",
	"rgbled_main",
	"adc_main",
	"tone_alarm_main",
	"trone_main",
	"uorb_main",
	"uavcan_main",
	"bl_update_main",
	"mixer_main",
	"motor_test_main",
	"mtd_main",
	"nshterm_main",
	"perf_main",
	"pwm_main",
	"reboot_main",
	"reflect_main",
	"usb_connected_main",
	"ver_main",
	"top_main",
	# interrupt handlers
	"irq_unexpected_isr",
	"up_interrupt_usart1",
	"up_interrupt_usart2",
	"up_interrupt_usart3",
	"up_interrupt_usart6",
	"up_interrupt_uart4",
	"up_interrupt_uart7",
	"up_interrupt_uart8",
	"stm32_i2c1_isr",
	"stm32_i2c2_isr",
	"stm32_rdyinterrupt",
	"stm32_exti0_isr",
	"stm32_exti1_isr",
	"stm32_exti2_isr",
	"stm32_exti3_isr",
	"stm32_exti4_isr",
	"stm32_exti95_isr",
	"stm32_exti1510_isr",
	"stm32_interrupt",
	"stm32_usbinterrupt",
	"stm32_dmainterrupt",
	"stm32_nmi",
	"stm32_busfault",
	"stm32_usagefault",
	"stm32_pendsv",
	"stm32_dbgmonitor",
	"stm32_reserved",
	"up_svcall",
	"up_hardfault",
	"up_memfault",
	"hrt_tim_isr",
	"_ZN6deviceL9interruptEiPv", # device::interrupt(int, void*)
	"_ZL13pwmin_tim_isriPv", # pwmin_tim_isr(int, void*)
	"_ZN12PX4IO_serial10_interruptEiPv", # PX4IO_serial::_interrupt(int, void*)
	"_ZL8can1_irqiPv", # can1_irq(int, void*)
	"_ZL8can2_irqiPv", # can2_irq(int, void*)
	# System functions
	"up_cxxinitialize",
	"__cxa_pure_virtual",
	"__start",
	"os_start",
	"task_start",
	"task_create",
	"thread_create",
	"pthread_start",
	"stm32_callback", 
	"task_atexit",
	"up_assert", 
	"_up_assert", 
	"up_initialize",
	"up_timerinit",
	"stm32_irqinfo",
	"stm32_clockconfig",
	"stm32_fpuconfig",
	"stm32_lowsetup",
	"stm32_gpioinit",
	"stm32_boardinitialize",
	"up_earlyserialinit",
	"up_initial_state",
	"sem_initialize",
	"task_initialize",
	"irq_initialize",
	"wd_initialize",
	"clock_initialize",
	"timer_initialize",
	"sig_initialize",
	"mq_initialize",
	"pthread_initialize",
	"fs_initialize",
	"net_initialize",
	"up_initialize",
	"lib_initialize",
	"group_initialize",
	"kmm_trysemaphore",
	"kmm_givesemaphore",
	"sched_garbagecollection",
	"up_idle",
	"os_bringup",
	"sched_process_timer",
	"up_idlepm",
	"up_irqinitialize",
	"up_timerinit",
	"up_hardfault",
	"stm32_prioritize_syscall",
	"up_memfault",
	"up_systemreset", # TODO: Make this a system call
	"mcu_version", # TODO: Make this a system call
	"_ZL10setbaseprij",
])

llvm_callgraph = None
indirect_callgraph = None

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
	global llvm_callgraph, indirect_callgraph
	global entry_name
	global print_callgraph, dot_callgraph
	global print_verbose

	desc = "Build a call graph that includes both direct and indirect calls"
	parser = argparse.ArgumentParser(description=desc)
	parser.add_argument("<llvm-callgraph>", type=argparse.FileType("r"),  
						help="Call graph dumped by 'opt -print-callgraph'")
	parser.add_argument("<indirect-callgraph>", type=argparse.FileType("r"),  
						help="Call graph dumped by 'llvm_indirect_calls.py'")
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
	llvm_callgraph = args["<llvm-callgraph>"]
	indirect_callgraph = args["<indirect-callgraph>"]
	entry_name = args["FUNCTION"]
	print_callgraph = args["print_callgraph"]
	dot_callgraph = args["dot_callgraph"]
	print_verbose = args["print_verbose"]

	if not print_callgraph and \
	   not dot_callgraph:
		print_callgraph = True

def main():
	check_args(sys.argv)

	# TODO: Handle multiple entries
	if entry_name in blacklist:
		blacklist.remove(entry_name)

	c.load(llvm_callgraph, blacklist)
	c.load(indirect_callgraph, blacklist)

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
