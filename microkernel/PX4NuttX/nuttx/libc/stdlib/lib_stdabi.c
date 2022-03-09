#include <stdlib.h>

void __aeabi_memcpy(void *dest, const void *src, size_t n)
{
  memcpy(dest, src, n);
}
 
void __aeabi_memcpy4(void *dest, const void *src, size_t n)
{
  memcpy(dest, src, n);
}

void __aeabi_memcpy8(void *dest, const void *src, size_t n)
{
  memcpy(dest, src, n);
}
