/****************************************************************************
 * sched/task_init.c
 *
 *   Copyright (C) 2007, 2009, 2013 Gregory Nutt. All rights reserved.
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
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <sched.h>
#include <errno.h>

#include <nuttx/arch.h>
#include <nuttx/memguard.h>

#include "os_internal.h"
#include "group_internal.h"

/****************************************************************************
 * Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Type Declarations
 ****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/

/****************************************************************************
 * Private Variables
 ****************************************************************************/

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: task_init
 *
 * Description:
 *   This function initializes a Task Control Block (TCB) in preparation for
 *   starting a new thread.  It performs a subset of the functionality of
 *   task_create()
 *
 *   Unlike task_create():
 *     1. Allocate the TCB.  The pre-allocated TCB is passed in the arguments.
 *     2. Allocate the stack.  The pre-allocated stack is passed in the arguments.
 *     3. Activate the task. This must be done by calling task_activate().
 *
 * Input Parameters:
 *   tcb        - Address of the new task's TCB
 *   name       - Name of the new task (not used)
 *   priority   - Priority of the new task
 *   stack      - Start of the pre-allocated stack
 *   stack_size - Size (in bytes) of the stack allocated
 *   entry      - Application start point of the new task
 *   arg        - A pointer to an array of input parameters. Up to
 *                CONFIG_MAX_TASK_ARG parameters may be provided.  If fewer
 *                than CONFIG_MAX_TASK_ARG parameters are passed, the list
 *                should be terminated with a NULL argv[] value. If no
 *                parameters are required, argv may be NULL.
 *
 * Return Value:
 *   OK on success; ERROR on failure with errno set appropriately.  (See
 *   task_schedsetup() for possible failure conditions).  On failure, the
 *   caller is responsible for freeing the stack memory and for calling
 *   sched_releasetcb() to free the TCB (which could be in most any state).
 *
 ****************************************************************************/

#ifndef CONFIG_CUSTOM_STACK
int task_init(FAR struct tcb_s *tcb, const char *name, int priority,
              FAR uint32_t *stack, uint32_t stack_size,
              main_t entry, FAR char * const argv[])
#else
int task_init(FAR struct tcb_s *tcb, const char *name, int priority,
              main_t entry, FAR char * const argv[])
#endif
{
  FAR struct task_tcb_s *ttcb = (FAR struct task_tcb_s *)tcb;
  int errcode;
  int ret;
#ifdef CONFIG_MEMGUARD_HEAP
  FAR void *heap_start;
  size_t heap_size;
#endif

  /* Only tasks and kernel threads can be initialized in this way */

#ifndef CONFIG_DISABLE_PTHREAD
  DEBUGASSERT(tcb &&
             (tcb->flags & TCB_FLAG_TTYPE_MASK) != TCB_FLAG_TTYPE_PTHREAD);
#endif

  /* Create a new task group */

#ifdef HAVE_TASK_GROUP
  ret = group_allocate(ttcb);
  if (ret < 0)
    {
      errcode = -ret;
      goto errout;
    }
#endif

 /* Associate file descriptors with the new task */

#if CONFIG_NFILE_DESCRIPTORS > 0 || CONFIG_NSOCKET_DESCRIPTORS > 0
  ret = group_setuptaskfiles(ttcb);
  if (ret < 0)
    {
      errcode = -ret;
      goto errout_with_group;
    }
#endif

#ifdef CONFIG_MEMGUARD_HEAP
  /* Allocate the heap for the TCB. */

  if (tcb->heap_alloc_ptr)
    {
#ifdef CONFIG_MEMGUARD_HEAP
      seclldbg("Heap release: heap=%p start=%p\n",
               &tcb->mmheap, tcb->heap_alloc_ptr);
#endif
      mm_free(&g_mmheap, tcb->heap_alloc_ptr);
      tcb->heap_alloc_ptr = NULL;
    }

  if (!tcb->heap_alloc_ptr)
    {
      // XXX: Let app developer decide heap size
      heap_size  = stack_size * 3;

#if defined(CONFIG_DEBUG) && !defined(CONFIG_DEBUG_STACK)
      heap_start = mm_zalloc(&g_mmheap, heap_size);
#else
      heap_start = mm_malloc(&g_mmheap, heap_size);
#endif

#ifdef CONFIG_DEBUG
      if (!heap_start)
        {
          secdbg("ERROR: Failed to allocate heap, size %d\n", heap_size);
          errcode = -ERROR;
          goto errout_with_group;
        }
#endif

      mm_initialize(&tcb->mmheap, heap_start, heap_size);
      tcb->heap_alloc_ptr = heap_start;
    }
#endif

  /* Configure the user provided stack region */

#ifndef CONFIG_CUSTOM_STACK
  up_use_stack(tcb, stack, stack_size);
#endif

  /* Initialize the task control block */

  ret = task_schedsetup(ttcb, priority, task_start, entry,
                        TCB_FLAG_TTYPE_TASK);
  if (ret < OK)
    {
      errcode = -ret;
      goto errout_with_group;
    }

  /* Setup to pass parameters to the new task */

  (void)task_argsetup(ttcb, name, argv);

#ifdef CONFIG_MEMGUARD_PROFILE
  if (tcb->pid >= MEMGUARD_PROFILE_COUNT) {
    seclldbg("Error: Increase MEMGUARD_PROFILE_COUNT, pid=%d\n", tcb->pid);
    errcode = -ENOMEM;
    goto errout_with_group;
  }
  g_memguard_profile_list[tcb->pid].start = (uintptr_t)entry;
  g_memguard_profile_list[tcb->pid].stack_ptr = (uintptr_t)stack;
  g_memguard_profile_list[tcb->pid].stack_size = stack_size;
  g_memguard_profile_list[tcb->pid].heap_size = 0;
  g_memguard_profile_list[tcb->pid].heap_max = 0;
#endif

  /* Now we have enough in place that we can join the group */

#ifdef HAVE_TASK_GROUP
  ret = group_initialize(ttcb);
  if (ret < 0)
    {
      errcode = -ret;
      goto errout_with_group;
    }
#endif
  return OK;

errout_with_group:
#ifdef HAVE_TASK_GROUP
  group_leave(tcb);

errout:
#endif
  set_errno(errcode);
  return ERROR;
}

