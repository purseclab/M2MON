# Chung Hwan Kim <chungkim@cs.purdue.edu>

#
# Build the memguard_test tool.
#

MODULE_COMMAND	 = memguard_test
SRCS		 = memguard_test.c

ifdef NOOPT
MAXOPTIMIZATION	 = -O0
else
MAXOPTIMIZATION	 = -Os
endif

MODULE_STACKSIZE = 1800
