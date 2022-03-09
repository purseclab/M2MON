/* Chung Hwan Kim <chungkim@cs.purdue.edu> */

#ifndef __ARCH_ARM_INCLUDE_MPU_H
#define __ARCH_ARM_INCLUDE_MPU_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#if defined(CONFIG_ARCH_CORTEXM3) || defined(CONFIG_ARCH_CORTEXM4)
#  include <arch/armv7-m/mpu.h>
#else
#  error architecture not supported
#endif

#ifdef CONFIG_ARMV7M_MPU

#ifndef __ASSEMBLY__

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/


/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Functions
 ****************************************************************************/


#undef EXTERN
#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */

#endif /* CONFIG_ARMV7M_MPU */
#endif /* __ARCH_ARM_INCLUDE_MPU_H */

