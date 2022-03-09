/************************************************************************************
 * arch/arm/src/armv7-m/mpu.h
 *
 *   Copyright (C) 2011, 2013 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ************************************************************************************/

#ifndef __ARCH_ARM_INCLUDE_ARMV7_M_MPU_H
#define __ARCH_ARM_INCLUDE_ARMV7_M_MPU_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
#  include <sys/types.h>
#  include <stdint.h>
#  include <stdbool.h>
#  include <debug.h>

#  include <nuttx/memguard.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* MPU Register Addresses */

#define MPU_TYPE                0xe000ed90 /* MPU Type Register */
#define MPU_CTRL                0xe000ed94 /* MPU Control Register */
#define MPU_RNR                 0xe000ed98 /* MPU Region Number Register */
#define MPU_RBAR                0xe000ed9c /* MPU Region Base Address Register */
#define MPU_RASR                0xe000eda0 /* MPU Region Attribute and Size Register */

/* MPU Type Register Bit Definitions */

#define MPU_TYPE_SEPARATE       (1 << 0) /* Bit 0: 0:unified or 1:separate memory maps */
#define MPU_TYPE_DREGION_SHIFT  (8)      /* Bits 8-15: Number MPU data regions */
#define MPU_TYPE_DREGION_MASK   (0xff << MPU_TYPE_DREGION_SHIFT)
#define MPU_TYPE_IREGION_SHIFT  (16)     /* Bits 16-23: Number MPU instruction regions */
#define MPU_TYPE_IREGION_MASK   (0xff << MPU_TYPE_IREGION_SHIFT)

/* MPU Control Register Bit Definitions */

#define MPU_CTRL_ENABLE         (1 << 0)  /* Bit 0: Enable the MPU */
#define MPU_CTRL_HFNMIENA       (1 << 1)  /* Bit 1: Enable MPU during hard fault, NMI, and FAULTMAS */
#define MPU_CTRL_PRIVDEFENA     (1 << 2)  /* Bit 2: Enable privileged access to default memory map */

/* MPU Region Number Register Bit Definitions */

#define MPU_RNR_MASK            (0xff)     

/* MPU Region Base Address Register Bit Definitions */

#define MPU_RBAR_REGION_SHIFT   (0)       /* Bits 0-3: MPU region */
#define MPU_RBAR_REGION_MASK    (15 << MPU_RBAR_REGION_SHIFT)
#define MPU_RBAR_VALID          (1 << 4)  /* Bit 4: MPU Region Number valid */ 
#define MPU_RBAR_ADDR_MASK      0xffffffe0 /* Bits N-31:  Region base addrese */

/* MPU Region Attributes and Size Register Bit Definitions */

#define MPU_RASR_ENABLE         (1 << 0)  /* Bit 0: Region enable */
#define MPU_RASR_SIZE_SHIFT     (1)       /* Bits 1-5: Size of the MPU protection region */  
#define MPU_RASR_SIZE_MASK      (31 << MPU_RASR_SIZE_SHIFT)
#  define MPU_RASR_SIZE_LOG2(n) ((n-1) << MPU_RASR_SIZE_SHIFT)
#define MPU_RASR_SRD_SHIFT      (8)       /* Bits 8-15: Subregion disable */
#define MPU_RASR_SRD_MASK       (0xff << MPU_RASR_SRD_SHIFT)
#  define MPU_RASR_SRD_0        (0x01 << MPU_RASR_SRD_SHIFT)
#  define MPU_RASR_SRD_1        (0x02 << MPU_RASR_SRD_SHIFT)
#  define MPU_RASR_SRD_2        (0x04 << MPU_RASR_SRD_SHIFT)
#  define MPU_RASR_SRD_3        (0x08 << MPU_RASR_SRD_SHIFT)
#  define MPU_RASR_SRD_4        (0x10 << MPU_RASR_SRD_SHIFT)
#  define MPU_RASR_SRD_5        (0x20 << MPU_RASR_SRD_SHIFT)
#  define MPU_RASR_SRD_6        (0x40 << MPU_RASR_SRD_SHIFT)
#  define MPU_RASR_SRD_7        (0x80 << MPU_RASR_SRD_SHIFT)
#define MPU_RASR_B              (1 << 16) /* Bit 16: Bufferable */
#define MPU_RASR_C              (1 << 17) /* Bit 17: Cacheable */
#define MPU_RASR_S              (1 << 18) /* Bit 18: Shareable */
#define MPU_RASR_ATTR_SHIFT     (19)      /* Bits 19-21: TEX Address Permisson */
#define MPU_RASR_ATTR_MASK      (7 << MPU_RASR_ATTR_SHIFT)
#define MPU_RASR_AP_SHIFT       (24)      /* Bits 24-26: Access permission */
#define MPU_RASR_AP_MASK        (7 << MPU_RASR_AP_SHIFT)
#  define MPU_RASR_AP_NONO      (0 << MPU_RASR_AP_SHIFT) /* P:None U:None */
#  define MPU_RASR_AP_RWNO      (1 << MPU_RASR_AP_SHIFT) /* P:RW   U:None */
#  define MPU_RASR_AP_RWRO      (2 << MPU_RASR_AP_SHIFT) /* P:RW   U:RO   */
#  define MPU_RASR_AP_RWRW      (3 << MPU_RASR_AP_SHIFT) /* P:RW   U:RW   */
#  define MPU_RASR_AP_RONO      (5 << MPU_RASR_AP_SHIFT) /* P:RO   U:None */
#  define MPU_RASR_AP_RORO      (6 << MPU_RASR_AP_SHIFT) /* P:RO   U:RO   */
#define MPU_RASR_XN             (1 << 28) /* Bit 28: Instruction access disable */

#if defined(CONFIG_MEMGUARD) || defined(CONFIG_MTRACE)
#define MPU_RASR_ATTR_TEX_0        (0 << MPU_RASR_ATTR_SHIFT)
#define MPU_RASR_ATTR_TEX_1        (1 << MPU_RASR_ATTR_SHIFT)
#define MPU_RASR_ATTR_TEX_2        (2 << MPU_RASR_ATTR_SHIFT)

#define MPU_RASR_STRONGLY_ORDERED  (MPU_RASR_ATTR_TEX_0)
#define MPU_RASR_DEVICE_SHARED     (MPU_RASR_ATTR_TEX_0|MPU_RASR_S)
#define MPU_RASR_NORMAL_WT         (MPU_RASR_ATTR_TEX_0|MPU_RASR_C)
#define MPU_RASR_NORMAL_WB         (MPU_RASR_ATTR_TEX_0|MPU_RASR_B)
#define MPU_RASR_NORMAL            (MPU_RASR_ATTR_TEX_1)
#define MPU_RASR_NORMAL_WBWA       (MPU_RASR_ATTR_TEX_1|MPU_RASR_C|MPU_RASR_B)
#define MPU_RASR_DEVICE_NOT_SHARED (MPU_RASR_ATTR_TEX_2)
#endif /* CONFIG_MEMGUARD || CONFIG_MTRACE */

/****************************************************************************
 * Global Function Prototypes
 ****************************************************************************/

#ifndef __ASSEMBLY__
#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Name: mpu_allocregion
 *
 * Description:
 *  Allocate the next region
 *
 ****************************************************************************/

unsigned int memguard mpu_allocregion(void);

/****************************************************************************
 * Name: mpu_log2regionceil
 *
 * Description:
 *   Determine the smallest value of l2size (log base 2 size) such that the
 *   following is true:
 *
 *   size <= (1 << l2size)
 *
 ****************************************************************************/

uint8_t memguard mpu_log2regionceil(size_t size);

/****************************************************************************
 * Name: mpu_log2regionfloor
 *
 * Description:
 *   Determine the largest value of l2size (log base 2 size) such that the
 *   following is true:
 *
 *   size >= (1 << l2size)
 *
 ****************************************************************************/

uint8_t memguard mpu_log2regionfloor(size_t size);

/****************************************************************************
 * Name: mpu_subregion
 *
 * Description:
 *   Given (1) the offset to the beginning of valid data, (2) the size of the
 *   memory to be mapped and (2) the log2 size of the mapping to use, determine
 *   the minimal sub-region set to span that memory region.
 *
 * Assumption:
 *   l2size has the same properties as the return value from
 *   mpu_log2regionceil()
 *
 ****************************************************************************/

uint32_t memguard mpu_subregion(uintptr_t base, size_t size, uint8_t l2size);

/****************************************************************************
 * Inline Functions
 ****************************************************************************/

/****************************************************************************
 * Name: mpu_showtype
 *
 * Description:
 *   Show the characteristics of the MPU
 *
 ****************************************************************************/

void memguard mpu_showtype(void);

/****************************************************************************
 * Name: mpu_control
 *
 * Description:
 *   Configure and enable (or disable) the MPU
 *
 ****************************************************************************/

void memguard mpu_control(bool enable, bool hfnmiena, bool privdefena);

/****************************************************************************
 * Name: mpu_userflash
 *
 * Description:
 *   Configure a region for user program flash
 *
 ****************************************************************************/

void memguard mpu_userflash(uintptr_t base, size_t size);

/****************************************************************************
 * Name: mpu_privflash
 *
 * Description:
 *   Configure a region for privileged program flash
 *
 ****************************************************************************/

void memguard mpu_privflash(uintptr_t base, size_t size);

/****************************************************************************
 * Name: mpu_userintsram
 *
 * Description:
 *   Configure a region as user internal SRAM
 *
 ****************************************************************************/

void memguard mpu_userintsram(uintptr_t base, size_t size);

/****************************************************************************
 * Name: mpu_privintsram
 *
 * Description:
 *   Configure a region as privileged internal SRAM
 *
 ****************************************************************************/

 void memguard mpu_privintsram(uintptr_t base, size_t size);

/****************************************************************************
 * Name: mpu_userextsram
 *
 * Description:
 *   Configure a region as user external SRAM
 *
 ****************************************************************************/

void memguard mpu_userextsram(uintptr_t base, size_t size);

/****************************************************************************
 * Name: mpu_privextsram
 *
 * Description:
 *   Configure a region as privileged external SRAM
 *
 ****************************************************************************/

void memguard mpu_privextsram(uintptr_t base, size_t size);

/****************************************************************************
 * Name: mpu_peripheral
 *
 * Description:
 *   Configure a region as privileged periperal address space
 *
 ****************************************************************************/

void memguard mpu_peripheral(uintptr_t base, size_t size);

#if defined(CONFIG_MEMGUARD) || defined(CONFIG_MTRACE)
void memguard mpu_region(unsigned int region, 
                         uintptr_t base, size_t size, uint32_t modes);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif  /* __ASSEMBLY__ */
#endif  /* __ARCH_ARM_INCLUDE_ARMV7_M_MPU_H */

