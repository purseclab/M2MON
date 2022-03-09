/* Chung Hwan Kim <chungkim@cs.purdue.edu> */

#include <px4_config.h>

#include <sys/types.h>
#include <stdio.h>

#include <nuttx/ftrace.h>

#ifdef CONFIG_FTRACE

static void print(void)
{
  uint16_t i;

  printf("PID: %d\n", CONFIG_FTRACE_TARGET_PID);

#if CONFIG_TASK_NAME_SIZE > 0
  printf("NAME: %s()\n", g_ftrace_target_name);
#endif

  printf("FUNCTIONS: %d\n", g_ftrace_count);

  for (i = 0; i < g_ftrace_count; ++i)
  {
    printf("%p ", g_ftrace_list[i].func);
  }
  printf("\n\n");
}

#endif

__EXPORT int ftrace_main(int argc, char *argv[]);

int ftrace_main(int argc, char *argv[])
{
  int ret;

#ifdef CONFIG_FTRACE
  print();
  ret = 0;
#else
  printf("CONFIG_FTRACE is not enabled in NuttX\n");
  ret = 1;
#endif

  return ret;
}


