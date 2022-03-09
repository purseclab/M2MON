/* Chung Hwan Kim <chungkim@cs.purdue.edu> */

#include <px4_config.h>

#include <sys/types.h>
#include <stdio.h>

#include <nuttx/memguard.h>

__EXPORT int memguard_test_main(int argc, char *argv[]);

int memguard_test_main(int argc, char *argv[])
{
  int ret;

#ifdef CONFIG_MEMGUARD
  ret = memguard_test(argc, argv);
#else
  printf("CONFIG_MEMGUARD is not enabled in NuttX\n");
  ret = 1;
#endif

  return ret;
}


