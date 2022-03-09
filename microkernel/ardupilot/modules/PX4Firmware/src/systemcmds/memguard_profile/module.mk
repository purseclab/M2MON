# Chung Hwan Kim <chungkim@cs.purdue.edu>

#
# Build the memguard_profile tool.
#

MODULE_COMMAND	 = memguard_profile
SRCS		 = memguard_profile.c

ifdef NOOPT
MAXOPTIMIZATION	 = -O0
else
MAXOPTIMIZATION	 = -Os
endif

MODULE_STACKSIZE = 1800
