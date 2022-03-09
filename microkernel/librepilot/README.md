#Minion for LibrePilot CC3D#

HOW TO BUILD FIRMWARE
=====================

# Reference 1: https://librepilot.atlassian.net/wiki/display/LPDOC/Linux+-+Building+and+Packaging
# Reference 2: https://librepilot.atlassian.net/wiki/display/LPDOC/Make+basics

# Host environment: Ubuntu 14.04.4 LTS (64-bit)

# If it’s your first time to build ardupilot in the host environment,
# then run the following to install the required packages.

sudo add-apt-repository ppa:librepilot/tools -y
sudo apt-get update -q
sudo apt-get install build-essential curl ccache debhelper git-core git-doc flex graphviz bison libudev-dev libusb-1.0-0-dev libsdl1.2-dev python libopenscenegraph-dev qt56-meta-minimal qt56svg qt56script qt56serialport qt56multimedia qt56translations qt56tools qt56quickcontrols libosgearth-dev openscenegraph-plugin-osgearth libc6-i386

# Clone the librepilot Git repository

git clone https://bitbucket.org/librepilot/librepilot.git librepilot
cd librepilot

# Check out version 16.09
# (Our implementation is based on this version)

git checkout 16.09 -b stable

# Build the firmware

make fw_coptercontrol_clean
make fw_coptercontrol

# The firmware images are here:

# ELF firmware image:
build/firmware/fw_coptercontrol/fw_coptercontrol.elf

# Stripped firmware image:
build/firmware/fw_coptercontrol/fw_coptercontrol.bin

# Bootloader image:
build/firmware/fw_coptercontrol/fw_coptercontrol.opfw

# Upload the firmware to the drone
# Make sure the drone is connected to the host machine through
# a USB cable.

make fw_coptercontrol_program


HOW TO BUILD FIRMWARE WITH MINION
=================================

make fw_coptercontrol_clean
MINION=YES make fw_coptercontrol


HOW TO BUILD FIRMWARE IN LLVM IR
================================

make fw_coptercontrol_clean
LLVM=YES make fw_coptercontrol


HOW TO GET ACCESS TO THE DEBUG CONSOLE
======================================

# Reference 1: https://opwiki.readthedocs.io/en/latest/user_manual/cc3d/cc3d.html
# Reference 2: https://librepilot.atlassian.net/wiki/display/LPDOC/Debugging+the+Flight+Code

screen -S console /dev/ttyUSB0 57600 8n1


HOW TO FLASH FIRMWARE USING ST-LINK
===================================

# If you have not installed stlink yet, then build and install stlink.

git clone git://github.com/texane/stlink.git
cd stlink
make
cd build/Release
sudo make install

# Flash the firmware and reset the target

st-flash write build/firmware/fw_coptercontrol/fw_coptercontrol.bin 0x8003000
st-flash reset


HOW TO DEBUG FIRMWARE USING ST-LINK
===================================

# If you have not created .gdbinit file yet, then create it.

# <path-to-librepilot>/.gdbinit
+ define reload
+ kill
+ monitor jtag_reset
+ load
+ end
+ target extended localhost:4242

# ~/.gdbinit
+ add-auto-load-safe-path <path-to-ardupilot>/.gdbinit

# Run the ST-Link GDB server

st-util

# Run GDB on a separate terminal

cgdb -d arm-none-eabi-gdb build/firmware/fw_coptercontrol/fw_coptercontrol.elf
(gdb) load
(gdb) run

