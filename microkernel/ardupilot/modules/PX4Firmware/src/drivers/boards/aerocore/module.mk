#
# Board-specific startup code for the AeroCore
#

SRCS		 = aerocore_init.c \
		   aerocore_pwm_servo.c \
		   aerocore_spi.c \
		   aerocore_led.c

ifdef NOOPT
MAXOPTIMIZATION	 = -O0
else
MAXOPTIMIZATION	 = -Os
endif
