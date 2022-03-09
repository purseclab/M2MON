#
# Makefile to build the IRLock driver.
#

MODULE_COMMAND	= irlock

SRCS		= irlock.cpp

ifdef NOOPT
MAXOPTIMIZATION	 = -O0
else
MAXOPTIMIZATION	 = -Os
endif
