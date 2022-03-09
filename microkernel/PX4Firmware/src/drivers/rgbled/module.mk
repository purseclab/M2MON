#
# TCA62724FMG driver for RGB LED
#

MODULE_COMMAND	 = rgbled

SRCS		 = rgbled.cpp

ifdef NOOPT
MAXOPTIMIZATION	 = -O0
else
MAXOPTIMIZATION	 = -Os
endif
