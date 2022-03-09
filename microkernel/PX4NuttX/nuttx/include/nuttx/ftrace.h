/* Chung Hwan Kim <chungkim@cs.purdue.edu> */

#ifndef __INCLUDE_NUTTX_FTRACE_H
#define __INCLUDE_NUTTX_FTRACE_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <queue.h>

#ifdef CONFIG_FTRACE

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

struct ftrace_s
{
  sq_entry_t node;
  FAR void *func;
};

typedef struct ftrace_s ftrace_t;

/****************************************************************************
 * Public Data
 ****************************************************************************/

EXTERN ftrace_t g_ftrace_list[CONFIG_FTRACE_MAX_FUNCTIONS];
EXTERN uint16_t g_ftrace_count;

#if CONFIG_TASK_NAME_SIZE > 0
EXTERN char     g_ftrace_target_name[CONFIG_TASK_NAME_SIZE];
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

EXTERN void notrace ftrace_initialize(void);

#if CONFIG_TASK_NAME_SIZE > 0
EXTERN void notrace ftrace_target_namesetup(pid_t pid, FAR const char *name);
#else
#define ftrace_target_namesetup(p,n)
#endif

EXTERN void notrace __cyg_profile_func_enter(void *callee, void *caller);
EXTERN void notrace __cyg_profile_func_exit(void *callee, void *caller);

#undef EXTERN
#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */

#endif /* CONFIG_FTRACE */

#endif /* __INCLUDE_NUTTX_FTRACE_H */
