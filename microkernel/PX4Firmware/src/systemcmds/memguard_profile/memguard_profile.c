/* Chung Hwan Kim <chungkim@cs.purdue.edu> */

#include <px4_config.h>

#include <sys/types.h>
#include <stdio.h>

#include <nuttx/memguard.h>

__EXPORT int memguard_profile_main(int argc, char *argv[]);

#ifdef CONFIG_MEMGUARD_PROFILE

static void print(void)
{
  uint16_t pid;

  for (pid = 0; pid < MEMGUARD_PROFILE_COUNT; ++pid)
  {
    if (!g_memguard_profile_list[pid].start)
      continue;
    if (g_memguard_profile_list[pid].start == (uintptr_t)memguard_profile_main)
      continue;
    printf("%d: start=%p stack_ptr=%p stack_size=%u heap_size=%u\n",
           pid,
           g_memguard_profile_list[pid].start,
           g_memguard_profile_list[pid].stack_ptr,
           g_memguard_profile_list[pid].stack_size,
           g_memguard_profile_list[pid].heap_max);
  }
  printf("\n");
}

#endif

int memguard_profile_main(int argc, char *argv[])
{
  int ret;

#ifdef CONFIG_MEMGUARD_PROFILE
  print();
  ret = 0;
#else
  printf("CONFIG_MEMGUARD_PROFILE is not enabled in NuttX\n");
  ret = 1;
#endif

  return ret;
}
