#
# RAMTRON file system driver
#

MODULE_COMMAND	 = mtd
SRCS		 = mtd.c 24xxxx_mtd.c

ifdef NOOPT
MAXOPTIMIZATION	 = -O0
else
MAXOPTIMIZATION	 = -Os
endif

EXTRACFLAGS	= -Wno-error

