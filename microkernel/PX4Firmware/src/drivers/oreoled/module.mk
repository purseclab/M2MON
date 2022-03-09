#
# Oreo LED driver
#

MODULE_COMMAND	 = oreoled
SRCS		 = oreoled.cpp

ifdef NOOPT
MAXOPTIMIZATION	 = -O0
else
MAXOPTIMIZATION	 = -Os
endif
