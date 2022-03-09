#JIGSAW Project#

HOW TO BUILD PIXHAWK FIRMWARE
=============================

# Reference: http://ardupilot.org/dev/docs/building-px4-for-linux-with-make.html
# Host environment: Ubuntu 14.04.4 LTS (64-bit)

# Clone the ardupilot Git repository

git clone https://github.com/ArduPilot/ardupilot.git ardupilot
cd ardupilot

# Check out ArduCopter-3.3.3
# (Our implementation is based on this version)

git checkout acf2e10cd912bacad172627e2797db701c032c6e -b ArduCopter-3.3.3

# If it’s your first time to build ardupilot in the host environment,
# then run the following to install the required packages

Tools/scripts/install-prereqs-ubuntu.sh -y
. ~/.profile

# If you ran the above script (install-prereqs-ubuntu.sh) to 
# install packages, I recommend to delete the whole cloned 
# repository, go back to the first step, and proceed from 
# there. Do not run install-prereqs-ubuntu.sh this time since
# you have already run it once. The script seems to write 
# files with root privilege and it prevents non-root users from 
# running the following steps correctly.

# Bring the submodules (PX4Firmware, PX4NuttX, and uavcan)
# in place

git submodule init
git submodule update

# Build the firmware

cd ArduCopter
make px4-v2-quad

# The firmware images are here:

# Compressed:
ArduCopter/ArduCopter-v2.px4

# Non-compressed with symbols:
modules/PX4Firmware/Build/px4fmu-v2_APM.build/firmware.elf

# Non-compressed without symbol:
modules/PX4Firmware/Build/px4fmu-v2_APM.build/firmware.bin

# Upload the firmware to the drone
# Make sure the drone is connected to the host machine through
# a USB cable.

make px4-v2-upload

# Or, use GDB if you have the debugging adapter 
# (BlackMagic Probe) installed
# See: http://ardupilot.org/dev/docs/debugging-with-gdb.html

cd modules/PX4Firmware
cgdb -d arm-none-eabi-gdb Build/px4fmu-v2_APM.build/firmware.elf
(gdb) load
(gdb) run

# If it’s your first time to use GDB in the host environment,
# then create a .gdbinit file in the PX4Firmware directory 
# and your home directory beforehand

# modules/PX4Firmware/.gdbinit:
+ target extended /dev/ttyACM0
+ monitor swdp_scan
+ attach 1
+ monitor vector_catch disable hard
+ set mem inaccessible-by-default off
+ monitor option erase
+ set print pretty

# ~/.gdbinit
+ add-auto-load-safe-path <path-to-ardupilot>/modules/PX4Firmware/.gdbinit

# Get access to the drone's system console (NSH) using a 
# debug cable
# See: http://ardupilot.org/dev/docs/interfacing-with-pixhawk-using-the-nsh.html

screen -S nsh /dev/ttyUSB0 57600 8n1

# Alternatively, use MAVProxy to get wireless access
sudo pip install mavproxy
mavproxy.py --master=/dev/ttyACM2
module load nsh
nsh start

# Run help to see what apps are there

nsh> help
help usage: help [-v] [<cmd>]
...


HOW TO ADD A NEW SYSTEM COMMAND
===============================

# Create a directory for a new system command
# (that is, a new app "hello") in PX4Firmware
# NOTE: hello can be our Blue Box if you replace its logic with
# the Blue Box's logic.

cd modules/PX4Firmware/src/systemcmds
mkdir hello
cd hello

# Create a new C source file in the directory; for example,

# modules/PX4Firmware/src/systemcmds/hello/hello.c:
+ #include <px4_config.h>
+ #include <sys/types.h>
+ #include <stdio.h>
+
+ __EXPORT int hello_main(int argc, char *argv[]);
+
+ int hello_main(int argc, char *argv[])
+ {
+ printf("hello, world!\n");
+ return 0;
+ }


# Write a Makefile (module.mk), so the build system can compile
# the new command while building the firmware

# modules/PX4Firmware/src/systemcmds/hello/module.mk:
+ MODULE_COMMAND = hello
+ SRCS = hello.c
+ MAXOPTIMIZATION = -Os
+ MODULE_STACKSIZE = 1800


# Add the new system command at the end of the
# "System commands" block in ardupilot’s Makefile for PX4

# mk/PX4/px4_common.mk:
...
#
# System commands
#
MODULES += systemcmds/bl_update
MODULES += systemcmds/mixer
...
MODULES += systemcmds/ver
+ MODULES += systemcmds/hello
...

# Build the firmware again to have the changes in effect

cd ArduCopter
make px4-clean
make px4-v2-quad

# Upload the firmware to the drone
# Make sure the drone is connected to the host machine through
# a USB cable.

make px4-v2-upload

# Or, use GDB if you have the debugging adapter installed

cd modules/PX4Firmware
cgdb -d arm-none-eabi-gdb Build/px4fmu-v2_APM.build/firmware.elf
(gdb) load
(gdb) run

# Get access to the drone's system console (NSH) using a 
# debug cable

screen -x nsh

# Or using MAVProxy

mavproxy.py --master=/dev/ttyACM2
module load nsh
nsh start

# You should be able to see the new system command in the help
# message and run it

nsh> help
help usage: help [-v] [<cmd>]
...
Builtin Apps:
...
hello
...

nsh> hello
hello, world!

HOW TO BUILD PIXHAWK FIRMWARE USING LLVM/CLANG
==============================================

cd ArduCopter
make px4-clean
LLVM=1 make px4-v2-quad
