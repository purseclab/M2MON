#
# PCA8574 driver for RGB LED
#

MODULE_COMMAND	 = pca8574
SRCS		 = pca8574.cpp

ifdef NOOPT
MAXOPTIMIZATION	 = -O0
else
MAXOPTIMIZATION	 = -Os
endif
