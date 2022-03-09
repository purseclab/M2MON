#!/bin/bash

ADDR2LINE="/opt/gcc-arm-none-eabi-4_9-2015q3/bin/arm-none-eabi-addr2line"
FIRMWARE="/local/debloat/src/ardupilot/modules/PX4Firmware/Build/px4fmu-v2_APM.build/firmware.elf"

$ADDR2LINE -e $FIRMWARE -i -p -f -C -a $@
