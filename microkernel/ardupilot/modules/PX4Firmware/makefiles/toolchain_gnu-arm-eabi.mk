#
#   Copyright (C) 2012-2014 PX4 Development Team. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 3. Neither the name PX4 nor the names of its contributors may be
#    used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

#
# Definitions for a generic GNU ARM-EABI toolchain
#

#$(info TOOLCHAIN  gnu-arm-eabi)

# Toolchain commands. Normally only used inside this file.
#
CROSSDEV		 = arm-none-eabi-

ifdef LLVM
GCC			 = $(CROSSDEV)gcc
CC			 = clang
CXX			 = clang++
CPP			 = clang -E
AR			 = llvm-ar rcs
LD			 = $(CROSSDEV)ld.gold -plugin=$(shell llvm-config --libdir)/LLVMgold.so
OBJDUMP			 = llvm-objdump
LLINK			 = llvm-link
else
CC			 = $(CROSSDEV)gcc
CXX			 = $(CROSSDEV)g++
CPP			 = $(CROSSDEV)gcc -E
AR			 = $(CROSSDEV)ar rcs
LD			 = $(CROSSDEV)ld
OBJDUMP			 = $(CROSSDEV)objdump
endif
NM			 = $(CROSSDEV)nm
OBJCOPY			 = $(CROSSDEV)objcopy

# Check if the right version of the toolchain is available
#
CROSSDEV_VER_SUPPORTED	 = 4.7.4 4.7.5 4.7.6 4.8.4 4.9.3
CROSSDEV_VER_FOUND	 = $(shell $(CC) -dumpversion)

ifndef LLVM
ifeq (,$(findstring $(CROSSDEV_VER_FOUND), $(CROSSDEV_VER_SUPPORTED)))
$(error Unsupported version of $(CC), found: $(CROSSDEV_VER_FOUND) instead of one in: $(CROSSDEV_VER_SUPPORTED))
endif
endif


# XXX this is pulled pretty directly from the fmu Make.defs - needs cleanup

ifdef NOOPT
MAXOPTIMIZATION		 ?= -O0
else
MAXOPTIMIZATION		 ?= -O3
endif

# Base CPU flags for each of the supported architectures.
#
ARCHCPUFLAGS_CORTEXM4F	 = -mcpu=cortex-m4 \
			   -mthumb \
			   -march=armv7e-m \
			   -mfpu=fpv4-sp-d16 \
			   -mfloat-abi=hard

ARCHCPUFLAGS_CORTEXM4	 = -mcpu=cortex-m4 \
			   -mthumb \
			   -march=armv7e-m \
			   -mfloat-abi=soft

ARCHCPUFLAGS_CORTEXM3	 = -mcpu=cortex-m3 \
			   -mthumb \
			   -march=armv7-m \
			   -mfloat-abi=soft

# Enabling stack checks if OS was build with them
#
TEST_FILE_STACKCHECK=$(WORK_DIR)nuttx-export/include/nuttx/config.h
TEST_VALUE_STACKCHECK=CONFIG_ARMV7M_STACKCHECK\ 1
ENABLE_STACK_CHECKS=$(shell $(GREP) -q "$(TEST_VALUE_STACKCHECK)" $(TEST_FILE_STACKCHECK); echo $$?;)
ifeq ("$(ENABLE_STACK_CHECKS)","0")
ARCHINSTRUMENTATIONDEFINES_CORTEXM4F = -finstrument-functions -ffixed-r10
ARCHINSTRUMENTATIONDEFINES_CORTEXM4  = -finstrument-functions -ffixed-r10
ARCHINSTRUMENTATIONDEFINES_CORTEXM3  =
else
ARCHINSTRUMENTATIONDEFINES_CORTEXM4F =
ARCHINSTRUMENTATIONDEFINES_CORTEXM4  =
ARCHINSTRUMENTATIONDEFINES_CORTEXM3  =
endif

# Pick the right set of flags for the architecture.
#
ifdef LLVM
ARCHCPUFLAGS		 = --target=arm-none-eabi \
			   $(ARCHCPUFLAGS_$(CONFIG_ARCH))
else
ARCHCPUFLAGS		 = $(ARCHCPUFLAGS_$(CONFIG_ARCH))
endif
ifeq ($(ARCHCPUFLAGS),)
$(error Must set CONFIG_ARCH to one of CORTEXM4F, CORTEXM4 or CORTEXM3)
endif

# Set the board flags
#
ifeq ($(CONFIG_BOARD),)
$(error Board config does not define CONFIG_BOARD)
endif
ARCHDEFINES		+= -DCONFIG_ARCH_BOARD_$(CONFIG_BOARD) -D__PX4_NUTTX

ifdef LLVM
# XXX: Make this more generic
# Obtained from "$(GCC) $(ARCHCPUFLAGS) -E -x c++ - -v < /dev/null"
GCCINCLUDES_CORTEXM4F	= -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include/c++/4.9.3 \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include/c++/4.9.3/arm-none-eabi/armv7e-m/fpu \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include/c++/4.9.3/backward \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/include \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/include-fixed \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include

GCCINCLUDES_CORTEXM4	= -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include/c++/4.9.3 \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include/c++/4.9.3/arm-none-eabi/armv7e-m \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include/c++/4.9.3/backward \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/include \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/include-fixed \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include

GCCINCLUDES_CORTEXM3	= -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include/c++/4.9.3 \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include/c++/4.9.3/arm-none-eabi/armv7-m \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include/c++/4.9.3/backward \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/include \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/include-fixed \
			   -isystem /opt/gcc-arm-none-eabi-4_9-2015q3/bin/../lib/gcc/arm-none-eabi/4.9.3/../../../../arm-none-eabi/include

GCCINCLUDES		 = $(GCCINCLUDES_$(CONFIG_ARCH))

ARCHINCLUDES	+= $(GCCINCLUDES)
ARCHXXINCLUDES	+= $(GCCINCLUDES)
endif

# optimisation flags
#
ARCHOPTIMIZATION	 = $(MAXOPTIMIZATION) \
			   -g3 \
			   -ffunction-sections \
			   -fdata-sections
ifdef LLVM
ARCHOPTIMIZATION	 += -emit-llvm \
			   -fno-builtin \
			   -fno-inline \
			   -fno-inline-functions \
			   -Xclang -disable-llvm-optzns
else
ARCHOPTIMIZATION	 += -fno-strict-aliasing \
			   -fomit-frame-pointer \
			   -funsafe-math-optimizations \
			   -fno-strength-reduce \
			   -fno-builtin-printf
endif

# enable precise stack overflow tracking
# note - requires corresponding support in NuttX
INSTRUMENTATIONDEFINES	 = $(ARCHINSTRUMENTATIONDEFINES_$(CONFIG_ARCH))

# Language-specific flags
#
ARCHCFLAGS		 = -std=gnu99
ARCHCXXFLAGS		 = -fno-exceptions -fno-rtti -std=gnu++0x -fno-threadsafe-statics -D__CUSTOM_FILE_IO__
ifdef LLVM_RECORD
ARCHCXXFLAGS	 += -Xclang -fdump-record-layouts
endif

# Generic warnings
#
ARCHWARNINGS		 = -Wall \
			   -Wextra \
			   -Werror \
			   -Wshadow \
			   -Wfloat-equal \
			   -Wpointer-arith \
			   -Wmissing-declarations \
			   -Wpacked \
			   -Wno-unused-parameter \
			   -Werror=format-security \
			   -Werror=array-bounds \
			   -Wfatal-errors \
			   -Werror=unused-variable \
			   -Werror=reorder \
			   -Werror=uninitialized \
			   -Werror=init-self
ifdef LLVM
ARCHWARNINGS		 += -Qunused-arguments \
			   -fcolor-diagnostics \
			   -Wformat=2
else
ARCHWARNINGS		 += -Wlogical-op \
			   -Wdouble-promotion \
			   -Wformat=1 \
			   -Werror=double-promotion \
			   -Werror=unused-but-set-variable
endif
#   -Werror=float-conversion - works, just needs to be phased in with some effort and needs GCC 4.9+
#   -Wcast-qual  - generates spurious noreturn attribute warnings, try again later
#   -Wconversion - would be nice, but too many "risky-but-safe" conversions in the code
#   -Wcast-align - would help catch bad casts in some cases, but generates too many false positives

# C-specific warnings
#
ARCHCWARNINGS		 = $(ARCHWARNINGS) \
			   -Wbad-function-cast \
			   -Wstrict-prototypes \
			   -Wmissing-prototypes \
			   -Wnested-externs
ifndef LLVM
ARCHCWARNINGS		 += -Wold-style-declaration \
			   -Wmissing-parameter-type
endif

# C++-specific warnings
#
ARCHWARNINGSXX		 = $(ARCHWARNINGS) \
			   -Wno-missing-field-initializers

# pull in *just* libm from the toolchain ... this is grody
ifdef LLVM
LIBM			:= $(shell $(GCC) $(ARCHCPUFLAGS) -print-file-name=libm.a)
else
LIBM			:= $(shell $(CC) $(ARCHCPUFLAGS) -print-file-name=libm.a)
endif
EXTRA_LIBS		+= $(LIBM)

# Flags we pass to the C compiler
#
CFLAGS			 = $(ARCHCFLAGS) \
			   $(ARCHCWARNINGS) \
			   $(ARCHOPTIMIZATION) \
			   $(ARCHCPUFLAGS) \
			   $(ARCHINCLUDES) \
			   $(INSTRUMENTATIONDEFINES) \
			   $(ARCHDEFINES) \
			   $(EXTRADEFINES) \
			   $(EXTRACFLAGS) \
			   -fno-common \
			   $(addprefix -I,$(INCLUDE_DIRS))

# Flags we pass to the C++ compiler
#
CXXFLAGS		 = $(ARCHCXXFLAGS) \
			   $(ARCHWARNINGSXX) \
			   $(ARCHOPTIMIZATION) \
			   $(ARCHCPUFLAGS) \
			   $(ARCHXXINCLUDES) \
			   $(INSTRUMENTATIONDEFINES) \
			   $(ARCHDEFINES) \
			   -DCONFIG_WCHAR_BUILTIN \
			   $(EXTRADEFINES) \
			   $(EXTRACXXFLAGS) \
			   $(addprefix -I,$(INCLUDE_DIRS))

# Flags we pass to the assembler
#
AFLAGS			 = $(CFLAGS) -D__ASSEMBLY__ \
			   $(EXTRADEFINES) \
			   $(EXTRAAFLAGS)

# Flags we pass to the linker
#
LDFLAGS			+= --warn-common \
			   --gc-sections \
			   $(EXTRALDFLAGS) \
			   $(addprefix -T,$(LDSCRIPT)) \
			   $(addprefix -L,$(LIB_DIRS))

# Compiler support library
#
ifdef LLVM
LIBGCC			:= $(shell $(GCC) $(ARCHCPUFLAGS) -print-libgcc-file-name)
else
LIBGCC			:= $(shell $(CC) $(ARCHCPUFLAGS) -print-libgcc-file-name)
endif

# Files that the final link depends on
#
LINK_DEPS		+= $(LDSCRIPT)

# Files to include to get automated dependencies
#
DEP_INCLUDES		 = $(subst .o,.d,$(OBJS))

# Compile C source $1 to object $2
# as a side-effect, generate a dependency file
#
define COMPILE
	@$(ECHO) "CC:      $1"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(CCACHE) $(CC) -MD -c $(CFLAGS) $(abspath $1) -o $2
endef

# Compile C++ source $1 to $2
# as a side-effect, generate a dependency file
#
ifdef LLVM_RECORD
define COMPILEXX
	@$(ECHO) "CXX:     $1"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(CCACHE) $(CXX) -MD -c $(CXXFLAGS) $(abspath $1) -o $2 > $2.layouts
endef
else
define COMPILEXX
	@$(ECHO) "CXX:     $1"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(CCACHE) $(CXX) -MD -c $(CXXFLAGS) $(abspath $1) -o $2
endef
endif

# Assemble $1 into $2
#
define ASSEMBLE
	@$(ECHO) "AS:      $1"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(CC) -c $(AFLAGS) $(abspath $1) -o $2
endef

# Produce partially-linked $1 from files in $2
#
ifdef LLVM
define PRELINK
	@$(ECHO) "PRELINK: $1"
	@$(MKDIR) -p $(dir $1)
	$(Q) $(LD) -plugin-opt=save-temps -r -Map $1.map -o $1 $2 && $(OBJCOPY) --localize-hidden $1
endef
else
define PRELINK
	@$(ECHO) "PRELINK: $1"
	@$(MKDIR) -p $(dir $1)
	$(Q) $(LD) -Ur -Map $1.map -o $1 $2 && $(OBJCOPY) --localize-hidden $1
endef
endif

# Update the archive $1 with the files in $2
#
define ARCHIVE
	@$(ECHO) "AR:      $2"
	@$(MKDIR) -p $(dir $1)
	$(Q) $(AR) $1 $2
endef

# Link the objects in $2 into the binary $1
#
ifdef LLVM
ifdef LLVM_RECORD
ifeq ($(APM_MODULE_DIR),)
define LINK
	@$(ECHO) "LINK:    $1"
	@$(MKDIR) -p $(dir $1)
	@#$(Q) $(LD) -plugin-opt=save-temps $(LDFLAGS) -Map $1.map -o $1 --start-group $2 $(LIBS) $(EXTRA_LIBS) $(LIBGCC) --end-group
	@$(Q) $(LD) -r --warn-common $(addprefix -L,$(LIB_DIRS)) -Map $1.map -o $1 $2 $(LIBS)
	@$(ECHO) "BITCODE: $1.bc"
	$(Q) $(LD) -plugin-opt=emit-llvm $(LDFLAGS) -Map $1.map -o $1.bc --start-group $2 $(LIBS) $(EXTRA_LIBS) $(LIBGCC) --end-group
	$(Q) $(LLINK) -o $1.bc $1.bc $(filter $(realpath $(shell find $(WORK_DIR) -type f -name '*.bc')),$(realpath $(addsuffix .bc,$(2))))
	@$(ECHO) "LAYOUTS: $1.layouts"
	$(Q) cat $(filter-out $1.layouts,$(shell find $(WORK_DIR) -type f -name '*.layouts')) > $1.layouts
endef
else
define LINK
	@$(ECHO) "LINK:    $1"
	@$(MKDIR) -p $(dir $1)
	@#$(Q) $(LD) -plugin-opt=save-temps $(LDFLAGS) -Map $1.map -o $1 --start-group $2 $(LIBS) $(EXTRA_LIBS) $(LIBGCC) --end-group
	@$(Q) $(LD) -r --warn-common $(addprefix -L,$(LIB_DIRS)) -Map $1.map -o $1 $2 $(LIBS)
	@$(ECHO) "BITCODE: $1.bc"
	$(Q) $(LD) -plugin-opt=emit-llvm $(LDFLAGS) -Map $1.map -o $1.bc --start-group $2 $(LIBS) $(EXTRA_LIBS) $(LIBGCC) --end-group
	$(Q) $(LLINK) -o $1.bc $1.bc $(filter $(realpath $(shell find $(WORK_DIR) -type f -name '*.bc')),$(realpath $(addsuffix .bc,$(2))))
	@$(ECHO) "LAYOUTS: $1.layouts"
	$(Q) cat $(filter-out $1.layouts,$(shell find $(WORK_DIR) -type f -name '*.layouts')) $(shell find $(UAVCAN_DIR) -type f -name '*.layouts') $(shell find $(APM_MODULE_DIR)/ArduCopter -type f -name '*.layouts') > $1.layouts
endef
endif
else
define LINK
	@$(ECHO) "LINK:    $1"
	@$(MKDIR) -p $(dir $1)
	@#$(Q) $(LD) -plugin-opt=save-temps $(LDFLAGS) -Map $1.map -o $1 --start-group $2 $(LIBS) $(EXTRA_LIBS) $(LIBGCC) --end-group
	@$(Q) $(LD) -r --warn-common $(addprefix -L,$(LIB_DIRS)) -Map $1.map -o $1 $2 $(LIBS)
	@$(ECHO) "BITCODE: $1.bc"
	$(Q) $(LD) -plugin-opt=emit-llvm $(LDFLAGS) -Map $1.map -o $1.bc --start-group $2 $(LIBS) $(EXTRA_LIBS) $(LIBGCC) --end-group
	$(Q) $(LLINK) -o $1.bc $1.bc $(filter $(realpath $(shell find $(WORK_DIR) -type f -name '*.bc')),$(realpath $(addsuffix .bc,$(2))))
endef
endif
else
define LINK
	@$(ECHO) "LINK:    $1"
	@$(MKDIR) -p $(dir $1)
	$(Q) $(LD) $(LDFLAGS) -Map $1.map -o $1 --start-group $2 $(LIBS) $(EXTRA_LIBS) $(LIBGCC) --end-group
endef
endif

# Convert $1 from a linked object to a raw binary in $2
#
define SYM_TO_BIN
	@$(ECHO) "BIN:     $2"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(OBJCOPY) -O binary $1 $2
endef

# Take the raw binary $1 and make it into an object file $2.
# The symbol $3 points to the beginning of the file, and $3_len
# gives its length.
#
# - compile an empty file to generate a suitable object file
# - relink the object and insert the binary file
# - extract the length
# - create const unsigned $3_len with the extracted length as its value and compile it to an object file
# - link the two generated object files together
# - edit symbol names to suit
#
# NOTE: exercise caution using this with absolute pathnames; it looks
#       like the MinGW tools insert an extra _ in the binary symbol name; e.g.
#	the path:
#
#	/d/px4/firmware/Build/px4fmu_default.build/romfs.img
#
#	is assigned symbols like:
#
#	_binary_d__px4_firmware_Build_px4fmu_default_build_romfs_img_size
#
#	when we would expect
#
#	_binary__d_px4_firmware_Build_px4fmu_default_build_romfs_img_size
#
define BIN_SYM_PREFIX
	_binary_$(subst /,_,$(subst .,_,$1))
endef
ifdef LLVM
define BIN_TO_OBJ
	@$(ECHO) "OBJ:     $2"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(ECHO) > $2.c
	$(call COMPILE,$2.c,$2.c.o)
	$(Q) $(LD) -plugin-opt=save-temps -r -o $2.bin.o $2.c.o -b binary $1
	$(Q) $(ECHO) "const unsigned int $3_len = 0x`$(NM) -p --radix=x $2.bin.o | $(GREP) $(call BIN_SYM_PREFIX,$1)_size$$ | $(GREP) -o ^[0-9a-fA-F]*`;" > $2.c
	$(call COMPILE,$2.c,$2.c.o)
	$(Q) $(LD) -plugin-opt=save-temps -r -o $2 $2.c.o $2.bin.o
	$(Q) $(OBJCOPY) $2 \
		--redefine-sym $(call BIN_SYM_PREFIX,$1)_start=$3 \
		--strip-symbol $(call BIN_SYM_PREFIX,$1)_size \
		--strip-symbol $(call BIN_SYM_PREFIX,$1)_end \
		--rename-section .data=.rodata
	$(Q) $(REMOVE) $2.c $2.c.o $2.bin.o
endef
else
define BIN_TO_OBJ
	@$(ECHO) "OBJ:     $2"
	@$(MKDIR) -p $(dir $2)
	$(Q) $(ECHO) > $2.c
	$(call COMPILE,$2.c,$2.c.o)
	$(Q) $(LD) -r -o $2.bin.o $2.c.o -b binary $1
	$(Q) $(ECHO) "const unsigned int $3_len = 0x`$(NM) -p --radix=x $2.bin.o | $(GREP) $(call BIN_SYM_PREFIX,$1)_size$$ | $(GREP) -o ^[0-9a-fA-F]*`;" > $2.c
	$(call COMPILE,$2.c,$2.c.o)
	$(Q) $(LD) -r -o $2 $2.c.o $2.bin.o
	$(Q) $(OBJCOPY) $2 \
		--redefine-sym $(call BIN_SYM_PREFIX,$1)_start=$3 \
		--strip-symbol $(call BIN_SYM_PREFIX,$1)_size \
		--strip-symbol $(call BIN_SYM_PREFIX,$1)_end \
		--rename-section .data=.rodata
	$(Q) $(REMOVE) $2.c $2.c.o $2.bin.o
endef
endif
