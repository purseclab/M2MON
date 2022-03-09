#include <string.h>
 
void __aeabi_memclr(void *dest, size_t n)
{
  memset(dest, 0, n);
}
 
void __aeabi_memclr4(void *dest, size_t n)
{
  memset(dest, 0, n);
}
 
void __aeabi_memclr8(void *dest, size_t n)
{
  memset(dest, 0, n);
}
 
void __aeabi_memset(void *dest, char c, size_t n)
{
  memset(dest, c, n);
}

void __aeabi_memmove(void *dest, const void *src, size_t n)
{
  memmove(dest, src, n);
}

void __aeabi_memmove4(void *dest, const void *src, size_t n)
{
  memmove(dest, src, n);
}
