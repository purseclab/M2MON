#
# Land detector
#

MODULE_COMMAND	= land_detector

SRCS		= land_detector_main.cpp \
              land_detector_params.c \
              LandDetector.cpp \
			  MulticopterLandDetector.cpp \
              FixedwingLandDetector.cpp

ifdef NOOPT
EXTRACXXFLAGS   = -Weffc++ -O0
else
EXTRACXXFLAGS   = -Weffc++ -Os
endif

# Startup handler, the actual app stack size is
# in the task_spawn command
MODULE_STACKSIZE = 1200
