/* Chung Hwan Kim <chungkim@cs.purdue.edu> */

#ifndef __INCLUDE_NUTTX_MTRACE_H
#define __INCLUDE_NUTTX_MTRACE_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef CONFIG_MTRACE

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

EXTERN void mtrace_initialize(void);
EXTERN bool mtrace_fault(uint8_t mfaults, uint32_t maddr, uint32_t *regs);

#undef EXTERN
#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */

#endif /* CONFIG_MTRACE */

#endif /* __INCLUDE_NUTTX_MTRACE_H */
