/* Chung Hwan Kim <chungkim@cs.purdue.edu> */

#ifndef __ARCH_ARM_INCLUDE_NVIC_H
#define __ARCH_ARM_INCLUDE_NVIC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

//#ifdef CONFIG_ARCH_FPU

#include <arch/armv7-m/nvic.h>

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

//#endif /* CONFIG_ARCH_FPU */
#endif /* __ARCH_ARM_INCLUDE_NVIC_H */

