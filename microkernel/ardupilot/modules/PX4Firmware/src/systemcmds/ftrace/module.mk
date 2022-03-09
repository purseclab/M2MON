# Chung Hwan Kim <chungkim@cs.purdue.edu>

#
# kernel function tracer
#

MODULE_COMMAND	 = ftrace
SRCS		 = ftrace.c

ifdef NOOPT
MAXOPTIMIZATION	 = -O0
else
MAXOPTIMIZATION	 = -Os
endif

MODULE_STACKSIZE = 1800
