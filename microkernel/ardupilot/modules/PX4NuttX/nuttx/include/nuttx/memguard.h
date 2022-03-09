/* Chung Hwan Kim <chungkim@cs.purdue.edu> */

#ifndef __INCLUDE_NUTTX_MEMGUARD_H
#define __INCLUDE_NUTTX_MEMGUARD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <debug.h>

#include <nuttx/arch.h>
#include <arch/irq.h>

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

#ifdef CONFIG_MEMGUARD
#define memguard      __attribute__((section(".memguard.text"), aligned(4)))
#define memguard_tst  __attribute__((section(".memguard.test"), aligned(4)))
#define memguard_bss  __attribute__((section(".memguard.bss"), aligned(4)))
#define memguard_data __attribute__((section(".memguard.data"), aligned(4)))
#else
#define memguard
#define memguard_bss
#define memguard_data
#endif

#ifdef CONFIG_MEMGUARD_PROFILE
#define MEMGUARD_PROFILE_COUNT (256)
#endif

#ifdef CONFIG_MEMGUARD

#define memguard_acl  __attribute__((section(".memguard.acl"), aligned(4)))

#define MEMGUARD_SYM_SIZE CONFIG_TASK_NAME_SIZE

#define nooptimize    __attribute__((optimize("O0")))

#define memguard_isb() __asm__ __volatile__ ("isb 0xF":::"memory")
#define memguard_dsb() __asm__ __volatile__ ("dsb 0xF":::"memory")

#define SYS_enable_irq    (8)
#define SYS_disable_irq   (9)
#define SYS_return_unpriv (10)

/****************************************************************************
 * Public Type Definitions
 ****************************************************************************/

struct memguard_segment_s
{
  uintptr_t base;
  size_t size;
  uint32_t mod;
};

typedef struct memguard_segment_s memguard_segment_t;

struct memguard_acl_s
{
  uintptr_t start;
  char sym[MEMGUARD_SYM_SIZE];
  memguard_segment_t *segments;
};

typedef struct memguard_acl_s memguard_acl_t;

#endif /* CONFIG_MEMGUARD */

#ifdef CONFIG_MEMGUARD_PROFILE

struct memguard_profile_s
{
  uintptr_t start;
  uintptr_t stack_ptr;
  size_t stack_size;
  size_t heap_size;
  size_t heap_max;
};

typedef struct memguard_profile_s memguard_profile_t;

#endif /* CONFIG_MEMGUARD_PROFILE */

/****************************************************************************
 * Public Data
 ****************************************************************************/

#ifdef MEMGUARD_TEST
EXTERN bool memguard_test_created;
#endif

#ifdef CONFIG_MEMGUARD_PROFILE
EXTERN memguard_profile_t g_memguard_profile_list[MEMGUARD_PROFILE_COUNT];
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef CONFIG_MEMGUARD

EXTERN void memguard memguard_initialize(void);
EXTERN void memguard memguard_cswitch(FAR struct task_tcb_s *tcb);

EXTERN void up_enable_irq(int irq);
EXTERN void up_disable_irq(int irq);
EXTERN void up_return_unpriv(void);

#ifdef MEMGUARD_TEST
EXTERN int  memguard_tst nooptimize memguard_test(int argc, char *argv[]);
#else
#define memguard_test(a, b)   (0)
#endif

#else /* CONFIG_MEMGUARD */

#define memguard_test(a, b)   (0)

#define memguard_initialize()

#endif /* CONFIG_MEMGUARD */

#undef EXTERN
#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */

#endif /* __INCLUDE_NUTTX_MEMGUARD_H */
