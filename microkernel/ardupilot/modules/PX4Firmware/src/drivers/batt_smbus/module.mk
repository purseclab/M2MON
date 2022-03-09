#
# driver for SMBus smart batteries
#

MODULE_COMMAND	 = batt_smbus
SRCS		 = batt_smbus.cpp

ifdef NOOPT
MAXOPTIMIZATION	 = -O0
else
MAXOPTIMIZATION	 = -Os
endif
