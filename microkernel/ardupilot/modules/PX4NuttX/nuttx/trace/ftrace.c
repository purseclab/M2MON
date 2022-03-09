/* Chung Hwan Kim <chungkim@cs.purdue.edu> */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <queue.h>
#include <debug.h>

#include <nuttx/sched.h>
#include <nuttx/ftrace.h>

#ifdef CONFIG_FTRACE

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

#define FTRACE_HASH_SLOTS (1 << CONFIG_FTRACE_HASH_BITS)
#define FTRACE_HASH(val)  (((unsigned long)val * 0x9e370001) >> \
                           (32 - CONFIG_FTRACE_HASH_BITS))

/****************************************************************************
 * Private Type Definitions
 ****************************************************************************/

struct ftrace_hash_s
{
  sq_queue_t list;
};

typedef struct ftrace_hash_s ftrace_hash_t;

/****************************************************************************
 * Private Data
 ****************************************************************************/

extern volatile dq_queue_t g_readytorun;

static ftrace_hash_t g_ftrace_hash_table[FTRACE_HASH_SLOTS];

static bool g_ftrace_initialized;
static bool g_ftrace_tracing;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static inline notrace 
FAR struct tcb_s *ftrace_sched_self(void)
{
  return (FAR struct tcb_s*)g_readytorun.head;
}

static inline notrace 
void ftrace_sq_addfirst(FAR sq_entry_t *node, sq_queue_t *queue)
{
  node->flink = queue->head;
  if (!queue->head)
  {
    queue->tail = node;
  }
  queue->head = node;
}

static inline notrace 
char *ftrace_strncpy(char *dest, const char *src, size_t n)
{
  char *ret = dest;     /* Value to be returned */
  char *end = dest + n; /* End of dest buffer + 1 byte */

  while ((dest != end) && (*dest++ = *src++) != '\0');
  while (dest != end) *dest++ = '\0';
  return ret;
}

static inline notrace 
FAR ftrace_t *ftrace_search(FAR void *func)
{
  FAR sq_queue_t *list;
  FAR sq_entry_t *node;
  FAR ftrace_t *entry;
  unsigned long index;
  
  index = FTRACE_HASH(func);
  //DEBUG_ASSERT(index < FTRACE_HASH_SLOTS);
  if (index >= FTRACE_HASH_SLOTS)
  {
    __asm__("BKPT");
  }

  list = &g_ftrace_hash_table[index].list;

  node = list->head;
  while (node)
  {
    entry = (ftrace_t *)node;
    if (entry->func == func)
    {
      return entry;
    }

    node = sq_next(node);
  } 

  return NULL;
}

static inline notrace
FAR ftrace_t *ftrace_add(FAR void *func)
{
  FAR sq_queue_t *list;
  FAR ftrace_t *entry;
  unsigned long index;

  if (g_ftrace_count >= CONFIG_FTRACE_MAX_FUNCTIONS)
  {
    return NULL;
  }

  index = FTRACE_HASH(func);
  //DEBUG_ASSERT(index < FTRACE_HASH_SLOTS);
  if (index >= FTRACE_HASH_SLOTS)
  {
    __asm__("BKPT");
  }

  list = &g_ftrace_hash_table[index].list;

  entry = &g_ftrace_list[g_ftrace_count++];

  entry->func = func;
  
  ftrace_sq_addfirst(&entry->node, list);

  return entry;
}

/****************************************************************************
 * Public Data
 ****************************************************************************/

ftrace_t g_ftrace_list[CONFIG_FTRACE_MAX_FUNCTIONS];
uint16_t g_ftrace_count;

#if CONFIG_TASK_NAME_SIZE > 0
char g_ftrace_target_name[CONFIG_TASK_NAME_SIZE];
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void ftrace_initialize(void)
{
  unsigned long i;

  g_ftrace_count = 0;

  for (i = 0; i < FTRACE_HASH_SLOTS; ++i)
  {
    sq_init(&g_ftrace_hash_table[i].list);
  }

#if CONFIG_TASK_NAME_SIZE > 0
  g_ftrace_target_name[0] = '\0';
#endif

  g_ftrace_tracing = false;

  g_ftrace_initialized = true;
}

#if CONFIG_TASK_NAME_SIZE > 0
void ftrace_target_namesetup(pid_t pid, FAR const char *name)
{
  if (pid == CONFIG_FTRACE_TARGET_PID)
  {
    ftrace_strncpy(g_ftrace_target_name, name, CONFIG_TASK_NAME_SIZE);
  }
}
#endif

void __cyg_profile_func_enter(void *func, void *caller)
{
  FAR struct tcb_s *tcb;
  FAR ftrace_t *entry;
  pid_t pid = 0;

  if (g_ftrace_initialized != true)
  {
    return;
  }

  if (g_ftrace_tracing == true)
  {
    return;
  }

  tcb = ftrace_sched_self();

  if (tcb)
  {
    pid = tcb->pid;
  }

  //tlldbg("%d: %p -> %p\n", pid, caller, func);

  if (pid != CONFIG_FTRACE_TARGET_PID)
  {
    goto out;
  }

  entry = ftrace_search(func);

  if (!entry)
  {
    entry = ftrace_add(func);

    if (!entry)
    {
      __asm__("BKPT");
      tlldbg("CONFIG_FTRACE_MAX_FUNCTIONS is too small\n");
      goto out;
    }
  }

out:
  g_ftrace_tracing = false;
}

void __cyg_profile_func_exit(void *func, void *caller) {}

#endif /* CONFIG_FTRACE */
