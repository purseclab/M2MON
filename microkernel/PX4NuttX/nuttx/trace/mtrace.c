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
#include <assert.h>

#include <arch/mpu.h>
#include <arch/nvic.h>

#include <nuttx/sched.h>
#include <nuttx/mtrace.h>

#ifdef CONFIG_MTRACE

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

#define mtrace_getreg32(a)   (*(volatile uint32_t *)(a))
#define mtrace_setreg32(v,a) (*(volatile uint32_t *)(a) = (v))

/****************************************************************************
 * Private Type Definitions
 ****************************************************************************/


/****************************************************************************
 * Private Data
 ****************************************************************************/

extern volatile dq_queue_t g_readytorun;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void mtrace_init_regions(bool protect)
{
  volatile uint32_t start = 0;
  volatile uint32_t end = 0;
  volatile uint32_t size = 0;
  uint8_t region = 0;
  uint32_t modes = 0;

#if 1
  /* Peripheral region */
  start  = 0x40000000;
  end    = 0x60000000;
  size   = end - start;
  region = mpu_allocregion();
  modes  = MPU_RASR_S|MPU_RASR_B|MPU_RASR_XN;
  if (protect) {
    modes |= MPU_RASR_AP_NONO;
  } else {
    modes |= MPU_RASR_AP_RWNO;
  }
  tlldbg("%s %d: %p-%p (%d)\n", 
         (protect) ? "protect" : "unprotect", region, start, end, size);
  mpu_region(region, (uintptr_t)start, size, modes);
#endif

#if 0
  /* Private peripheral bus (NVIC, system timer, control block) region */
  start  = 0xE0000000;
  end    = 0xE0100000;
  size   = end - start;
  region = mpu_allocregion();
  modes  = MPU_RASR_STRONGLY_ORDERED|MPU_RASR_XN;
  if (protect) {
    modes |= MPU_RASR_AP_NONO;
  } else {
    modes |= MPU_RASR_AP_RWNO;
  }
  tlldbg("%s %d: %p-%p (%d)\n", 
         (protect) ? "protect" : "unprotect", region, start, end, size);
  mpu_region(region, (uintptr_t)start, size, modes);

  /* Memory mapped (STM32 standard) peripherals region */
  start  = 0xE0100000;
  end    = 0xFFFFFFFF;
  size   = end - start;
  region = mpu_allocregion();
  modes  = MPU_RASR_S|MPU_RASR_B|MPU_RASR_XN;
  if (protect) {
    modes |= MPU_RASR_AP_NONO;
  } else {
    modes |= MPU_RASR_AP_RWNO;
  }
  tlldbg("%s %d: %p-%p (%d)\n", 
         (protect) ? "protect" : "unprotect", region, start, end, size);
  mpu_region(region, (uintptr_t)start, size, modes);
#endif
}

static void mtrace_init_mpu(void)
{
  /* Enable non-base thread mode (NONBASETHRDENA) */
  mtrace_setreg32(mtrace_getreg32(NVIC_CFGCON) | 1, NVIC_CFGCON);

  /* Disable the MPU */
  mpu_control(false, false, false);

  /* Show MPU information */
  mpu_showtype();

  /* Initialize static MPU regions */
  mtrace_init_regions(true);

  /* Enable the MPU */
  mpu_control(true, false, true);
}

static inline FAR struct tcb_s *mtrace_sched_self(void)
{
  return (FAR struct tcb_s*)g_readytorun.head;
}

static inline bool mtrace_protected(uint32_t maddr)
{
  return (maddr >= 0x40000000 && maddr < 0x60000000);
}

/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Functions
 ****************************************************************************/

void mtrace_initialize(void)
{
  /* Disable IRQs. */
  irqdisable();

  /* Initialize memory protection. */
  mtrace_init_mpu();

  /* Enable IRQs. */
  irqenable();
}

bool mtrace_fault(uint8_t mfaults, uint32_t maddr, uint32_t *regs)
{
  FAR struct tcb_s *tcb;
  pid_t pid = 0;
  char *name = "";
 
  /* No memory fault? */
  if (!mfaults) {
    return false; /* Let the OS handle it. */
  }
 
  /* Unprotected memory region? */
  if (!mtrace_protected(maddr)) {
    return false;
  }
 
  tcb = mtrace_sched_self();
 
  if (tcb) {
    pid = tcb->pid;
#if CONFIG_TASK_NAME_SIZE > 0
    name = tcb->name;
#endif
  }
 
  /* Dump illegal memory access. */
  tlldbg("%d <%s>: %08x -> %08x\n", pid, name, regs[REG_PC], maddr);
   
  /* Unprotect memory regions temporarily. */
  mtrace_init_regions(false);
 
  /* Put the processor in single-step mode. */
  /* XXX: This single-steps the next instruction in Handler mode but not 
   * Thread mode. */
  mtrace_setreg32(0xa05f0005, NVIC_DHCSR);
 
  return true; /* Let the OS single-step the faulting instruction. */
}

#endif /* CONFIG_MTRACE */
