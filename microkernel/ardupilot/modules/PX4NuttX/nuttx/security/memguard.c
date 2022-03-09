/* Chung Hwan Kim <chungkim@cs.purdue.edu> */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <debug.h>
#include <assert.h>

#include <arch/mpu.h>
#include <arch/irq.h>
#include <arch/nvic.h>
#include <arch/syscall.h>

#include <nuttx/memguard.h>

#ifdef CONFIG_MEMGUARD

//#define MEMGUARD_PERF

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

#define RAM_VECTORS   (128)
#define ram_vectors   __attribute__((section(".ram_vectors"), aligned(4)))

#define memguard_getreg32(a)   (*(volatile uint32_t *)(a))
#define memguard_setreg32(v,a) (*(volatile uint32_t *)(a) = (v))

/****************************************************************************
 * Private Type Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Data
 ****************************************************************************/

/* MemGuard sections */
extern const uint32_t _smemguard_text;
extern const uint32_t _ememguard_text;
extern const uint32_t _smemguard_test;
extern const uint32_t _ememguard_test;
extern const uint32_t _smemguard_config;
extern const uint32_t _ememguard_config;
extern const uint32_t _smemguard_data_src;
extern const uint32_t _smemguard_data;
extern const uint32_t _ememguard_data;
extern const uint32_t _smemguard_bss;
extern const uint32_t _ememguard_bss;
extern const uint32_t _smemguard_stack;
extern const uint32_t _ememguard_stack;
extern const uint32_t _smemguard_stack_top;

/* Other sections */
extern const uint32_t _svectors;
extern const uint32_t _evectors;
extern const uint32_t _stext;
extern const uint32_t _etext;
extern const uint32_t _sram_vectors;
extern const uint32_t _eram_vectors;
extern const uint32_t _eronly;
extern const uint32_t _sdata;
extern const uint32_t _edata;
extern const uint32_t _sbss;
extern const uint32_t _ebss;
extern const uint32_t _esram;

/* Access control lists */
extern const volatile memguard_acl_t g_acls[];
extern const volatile uint8_t g_aclcount;

/* The next available MPU region number */
extern uint8_t g_region;

/* Vector address table in sram */
uint32_t ram_vectors g_ram_vectors[RAM_VECTORS];

/* Original vector address table in flash */
uint32_t *memguard_bss g_org_vectors;

#ifdef MEMGUARD_TEST
/* Indicates whether test task has been created */
bool memguard_test_created = false;
#endif

#endif /* CONFIG_MEMGUARD */

#ifdef CONFIG_MEMGUARD_PROFILE
memguard_profile_t g_memguard_profile_list[MEMGUARD_PROFILE_COUNT];
#endif

//extern void __up_enable_irq(int irq);

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#ifdef CONFIG_MEMGUARD

static inline uint32_t memguard memguard_getpsp(void)
{
  register uint32_t regval;
  __asm__ __volatile__ ("MRS %0, psp\n"  : "=r" (regval) );
  return regval;
}

static inline void memguard memguard_setpsp(uint32_t stack_top)
{
  __asm__ __volatile__ ("MSR psp, %0\n" : : "r" (stack_top) : "sp");
}

static inline uint32_t memguard memguard_getmsp(void)
{
  register uint32_t regval;
  __asm__ __volatile__ ("MRS %0, msp\n"  : "=r" (regval) );
  return regval;
}

static inline void memguard memguard_setmsp(uint32_t stack_top)
{
  __asm__ __volatile__ ("MSR msp, %0\n" : : "r" (stack_top) : "sp");
}

static void memguard memguard_show_map(void)
{
#ifdef CONFIG_DEBUG_SECURITY
  const uint32_t *start;
  const uint32_t *end;

  start = &_svectors;
  end   = &_evectors;
  seclldbg(".vectors = %p-%p (%d)\n", start, end, end-start);

  start = &_stext;
  end   = &_etext;
  seclldbg(".text = %p-%p (%d)\n", start, end, end-start);

  start = &_smemguard_text;
  end   = &_ememguard_text;
  seclldbg(".memguard.text = %p-%p (%d)\n", start, end, end-start);

  start = &_smemguard_config;
  end   = &_ememguard_config;
  seclldbg(".memguard.config = %p-%p (%d)\n", start, end, end-start);

  seclldbg("_smemguard_data_src = %p\n", &_smemguard_data_src);
  seclldbg("_eronly = %p\n", &_eronly);

  start = &_smemguard_data;
  end   = &_ememguard_data;
  seclldbg(".memguard.data = %p-%p (%d)\n", start, end, end-start);

  start = &_smemguard_bss;
  end   = &_ememguard_bss;
  seclldbg(".memguard.bss = %p-%p (%d)\n", start, end, end-start);

  start = &_smemguard_stack;
  end   = &_ememguard_stack;
  seclldbg(".memguard.stack = %p-%p (%d)\n", start, end, end-start);
  seclldbg("_smemguard_stack_top = %p\n", &_smemguard_stack_top);

  start = &_sram_vectors;
  end   = &_eram_vectors;
  seclldbg(".ram.vectors = %p-%p (%d)\n", start, end, end-start);
  seclldbg("g_ram_vectors = %p\n", g_ram_vectors);

  start = &_sdata;
  end   = &_edata;
  seclldbg(".data = %p-%p (%d)\n", start, end, end-start);

  start = &_sbss;
  end   = &_ebss;
  seclldbg(".bss = %p-%p (%d)\n", start, end, end-start);
  
  seclldbg("_esram = %p\n", &_esram);
#endif
}

static void memguard memguard_show_acls(void)
{
#ifdef CONFIG_DEBUG_SECURITY
  uint8_t i, j;
  volatile memguard_acl_t *acl;
  volatile memguard_segment_t *segment;

  seclldbg("# of ACLs: %d\n", g_aclcount);

  for (i = 0; i < g_aclcount; i++)
  {
    acl = (volatile memguard_acl_t *)&g_acls[i];
    seclldbg("%08x: %s\n", acl->start, acl->sym);
    for (j = 0; j < 8; j++)
    {
      segment = (volatile memguard_segment_t *)&acl->segments[j];
      seclldbg("  %d: %08x-%08x %d\n", 
               j, segment->base, segment->base + segment->size, segment->mod);
    }
  }
#endif
}

static void memguard memguard_protect(const volatile memguard_acl_t *acl)
{
  uint8_t i;
  volatile memguard_segment_t *segment;
  volatile uint32_t modes;
  unsigned int region;

  g_region = 0;

  for (i = 0; i < 8; i++)
  {
    segment = (volatile memguard_segment_t *)&acl->segments[i];
  
    modes = MPU_RASR_S|MPU_RASR_B;

    if ((segment->mod & ~1) & 2) // Write?
    {
      modes |= MPU_RASR_AP_RWRW;
    }
    else if ((segment->mod & ~1) & 4) // Read?
    {
      modes |= MPU_RASR_AP_RWRO;
    }
    else
    {
      modes |= MPU_RASR_AP_RWNO;
    }

    if (segment->mod & 1) // Execute?
    {
      modes |= MPU_RASR_XN;
    }

    region = mpu_allocregion();
    mpu_region(region, segment->base, segment->size, modes);
  }
}

#ifdef MEMGUARD_TEST
static void memguard memguard_protect_test(unsigned int region, bool protect)
{
  volatile uint32_t start = (uint32_t)&_smemguard_test;
  volatile uint32_t end = (uint32_t)&_ememguard_test;
  volatile uint32_t size = end - start;

  uint32_t modes;
  if (protect)
  {
    modes = MPU_RASR_AP_RORO|MPU_RASR_XN;
  }
  else
  {
    modes = MPU_RASR_AP_RORO;
  }

  mpu_region(region, (uintptr_t)start, size, modes);
}
#endif

static void memguard memguard_init_regions(void)
{
  volatile uint32_t start = 0;
  volatile uint32_t end = 0;
  volatile uint32_t size = 0;
  uint8_t region = 0;

  /* Accesses to the following regions use the default system address map
   * regardless of the MPU configuration:
   * (1) Vector Address Table (VAT)
   * (2) Private Peripheral Bus (PPB). */

  g_region = 0;

#if 0
  /* MemGuard code+config region */
  start  = (uint32_t)&_smemguard_text;
  end    = (uint32_t)&_ememguard_config;
  size   = end - start;
  region = mpu_allocregion();
  seclldbg("mpu region %d: %p-%p (%d)\n", region, start, end, size);
  mpu_region(region, (uintptr_t)start, size, MPU_RASR_AP_RONO);
  
  /* MemGuard data+stack region */
  start  = (uint32_t)&_smemguard_data;
  end    = (uint32_t)&_ememguard_stack;
  size   = end - start;
  region = mpu_allocregion();
  seclldbg("mpu region %d: %p-%p (%d)\n", region, start, end, size);
  mpu_region(region, (uintptr_t)start, end-start, MPU_RASR_AP_RWNO);
#endif

  /* Non-MemGuard ccsram region (used as heap and for board UUID) */
  start  = 0x10000000;
  end    = 0x20000000;
  size   = end - start;
  region = g_region;
  //seclldbg("mpu region %d: %p-%p (%d)\n", region, start, end, size);
  mpu_userintsram((uintptr_t)start, size);

  /* Non-MemGuard peripheral region */
  start  = 0x40000000;
  end    = 0x60000000;
  size   = end - start;
  region = mpu_allocregion();
  //seclldbg("mpu region %d: %p-%p (%d)\n", region, start, end, size);
  mpu_region(region, (uintptr_t)start, size, 
             MPU_RASR_S|MPU_RASR_B|MPU_RASR_AP_RWRW|MPU_RASR_XN);

  /* Non-MemGuard vectors+data+stack region */
  start  = (uint32_t)&_sram_vectors;
  end    = (uint32_t)&_esram;
  size   = end - start;
  region = g_region;
  //seclldbg("mpu region %d: %p-%p (%d)\n", region, start, end, size);
  mpu_userintsram((uintptr_t)start, size);

  /* Non-MemGuard code region */
  start  = (uint32_t)&_stext;
  end    = (uint32_t)&_smemguard_text;
  size   = end - start;
  region = g_region;
  //seclldbg("mpu region %d: %p-%p (%d)\n", region, start, end, size);
  mpu_userflash((uintptr_t)start, size);

#ifdef MEMGUARD_TEST
  /* MemGuard test region */
  start  = (uint32_t)&_smemguard_test;
  end    = (uint32_t)&_ememguard_test;
  size   = end - start;
  region = mpu_allocregion();
  //seclldbg("mpu region %d: %p-%p (%d)\n", region, start, end, size);
  memguard_protect_test(region, true);
#endif
}

static void memguard memguard_init_mpu(void)
{
  /* Enable non-base thread mode (NONBASETHRDENA) */
  memguard_setreg32(memguard_getreg32(NVIC_CFGCON) | 1, NVIC_CFGCON);

  /* Disable the MPU */
  mpu_control(false, false, false);

#ifdef CONFIG_DEBUG_SECURITY
  /* Show MPU information */
  mpu_showtype();
#endif

  /* Initialize static MPU regions */
  memguard_init_regions();

  /* Enable the MPU */
#if 0
  mpu_control(true, false, true);
#else
  mpu_control(false, false, false);
#endif
}

static void memguard memguard_init(void)
{
  const uint32_t *src;
  uint32_t *dest;

  /* Reset MemGuard bss */
  for (dest = (uint32_t *)&_smemguard_bss; dest < &_ememguard_bss; )
  {
    *dest++ = 0;
  }

  /* Initialize MemGuard data */
  for (src = &_smemguard_data_src, dest = (uint32_t *)&_smemguard_data; 
       dest < &_ememguard_data; )
  {
    *dest++ = *src++;
  }
  
  /* Reset vector address table in sram */
  for (dest = (uint32_t *)&_sram_vectors; dest < &_eram_vectors; )
  {
    *dest++ = 0;
  }

  /* Make a copy of the original vector address table */
  for (src = &_svectors, dest = (uint32_t *)g_ram_vectors; 
       src < &_evectors; )
  {
    *dest++ = *src++;
  }

  /* Update msp in the vector address table in sram */
  g_ram_vectors[0] = (uint32_t)&_smemguard_stack_top;

  /* Show memory map */
  memguard_show_map();

  /* Show access control list */
  memguard_show_acls();
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef MEMGUARD_PERF
enum perf_counter_type { PC_COUNT, PC_ELAPSED, PC_INTERVAL };
struct perf_ctr_header;
typedef struct perf_ctr_header *perf_counter_t;

extern perf_counter_t perf_alloc(enum perf_counter_type type, const char *name);
extern void perf_free(perf_counter_t handle);
extern void perf_begin(perf_counter_t handle);
extern void perf_end(perf_counter_t handle);

static volatile perf_counter_t _init_perf;
static volatile perf_counter_t _switch_view_perf;
static volatile perf_counter_t _enable_irq_perf;
static volatile perf_counter_t _disable_irq_perf;
#endif

void memguard_initialize(void)
{
#ifdef MEMGUARD_PERF
  _init_perf = perf_alloc(PC_ELAPSED, "memguard_init");
  _switch_view_perf = perf_alloc(PC_ELAPSED, "memguard_switch_view");
  _enable_irq_perf = perf_alloc(PC_ELAPSED, "memguard_enable_irq");
  _disable_irq_perf = perf_alloc(PC_ELAPSED, "memguard_disable_irq");

  perf_begin(_init_perf);
#endif

  /* Initialize MemGuard */
  memguard_init();

  /* Disable IRQs to perform atomic swaps */
  irqdisable();

  /* XXX: Do we trust OS's interrupt handlers or not? 
   * If we don't, then we should move the interrupt handler code into 
   * .memguard.test (we are currently only protecting the vector address 
   * table in sram. 
   * Otherwise, we have to make sure the interrupt handlers are executed in
   * unprivileged mode (currently the mode is switched to privileged mode
   * by the hardware when an interrupt occurs). */
#if 0
  /* Swap vector address tables */
  g_org_vectors = (uint32_t *)memguard_getreg32(NVIC_VECTAB);
  seclldbg("original vectors: %p\n", g_org_vectors);
  memguard_setreg32((uint32_t)g_ram_vectors, NVIC_VECTAB);
  seclldbg("new vectors: %p\n", g_ram_vectors);
#endif

  /* Swap stack pointers */
  seclldbg("original msp: %p\n", memguard_getmsp());
  memguard_setpsp(memguard_getmsp());
  memguard_setmsp((uint32_t)&_smemguard_stack_top);
  seclldbg("new psp: %p\n", memguard_getpsp());
  seclldbg("new msp: %p\n", memguard_getmsp());

  /* Enable IRQs after swapping */
  irqenable();

  /* Initialize memory protection */
  memguard_init_mpu();
  seclldbg("mpu initialized\n");

  /* Switch to unprivileged mode */
  setcontrol(getcontrol() | 3);
  memguard_isb();
  memguard_dsb();
  seclldbg("switched to unprivileged mode\n");

#ifdef MEMGUARD_PERF
  perf_end(_init_perf);
#endif
}

void memguard memguard_cswitch(FAR struct task_tcb_s *tcb)
{
  uint8_t i;
  volatile memguard_acl_t *acl;

  if (tcb == NULL)
  {
    return;
  }

#ifdef MEMGUARD_PERF
  perf_begin(_switch_view_perf);
#endif

  // XXX: Improve the search performance
  for (i = 0; i < g_aclcount; i++)
  {
    acl = (volatile memguard_acl_t *)&g_acls[i];
    if (acl->start == (uintptr_t)tcb->cmn.entry.main)
    {
      memguard_protect(acl);
#ifdef MEMGUARD_PERF
      perf_end(_switch_view_perf);
#endif
      return;
    }
  }

  memguard_init_regions();

#ifdef MEMGUARD_TEST
  if (memguard_test_created)
  {
    if (tcb->cmn.entry.main == memguard_test)
    {
      memguard_protect_test(g_region, false);
    }
    else
    {
      memguard_protect_test(g_region, true);
    }
  }
#endif

#ifdef MEMGUARD_PERF
  perf_end(_switch_view_perf);
#endif
}

void up_enable_irq(int irq)
{
  if (up_interrupt_context())
  {
    __up_enable_irq(irq);
  }
  else
  {
#ifdef MEMGUARD_PERF
    perf_begin(_enable_irq_perf);
#endif
    sys_call1(SYS_enable_irq, irq);
#ifdef MEMGUARD_PERF
    perf_end(_enable_irq_perf);
#endif
  }
}

void up_disable_irq(int irq)
{
  if (up_interrupt_context())
  {
    __up_disable_irq(irq);
  }
  else
  {
#ifdef MEMGUARD_PERF
    perf_begin(_disable_irq_perf);
#endif
    sys_call1(SYS_disable_irq, irq);
#ifdef MEMGUARD_PERF
    perf_end(_disable_irq_perf);
#endif
  }
}

void up_return_unpriv(void)
{
  sys_call0(SYS_return_unpriv);
}

#ifdef MEMGUARD_TEST

/****************************************************************************
 * Test Functions
 ****************************************************************************/

/* This function represents the shadow reference system. */
int memguard_test(int argc, char *argv[])
{
  const uint8_t  len   = 8;
  const uint32_t start = (uint32_t)&_sdata;
  const uint32_t end   = (uint32_t)&_edata;
  const uint32_t size  = end - start - len;
  uint8_t *buf;
  uint8_t i;
    
  printf("[SHADOW] %p-%p (%d)\n", start, end, size);

  srand(time(NULL));

  while (true)
  {
    /* Choose N bytes at random location. */
    buf = (uint8_t *)(start + (rand() % size));

    /* Hex-dump chosen bytes. */
    printf("[SHADOW] %p:", buf);
    for (i = 0; i < len; i++)
    {
      printf(" %02x", buf[i]);
    }
    printf(" | ");
    for (i = 0; i < len; i++)
    {
      printf("%c", buf[i]);
    }
    printf("\n");

    /* Sleep before repeating. */
    sleep(1);
  }

  return 0;
}

#endif /* MEMGUARD_TEST */

#endif /* CONFIG_MEMGUARD */
