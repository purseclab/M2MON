#define HRT_TIMER_BASE       0x40012c00
#define REG(_reg)             (*(volatile unsigned *)(HRT_TIMER_BASE + _reg))
#define STM32_GTIM_CR1_OFFSET 0x0000
#define rCR1                  REG(STM32_GTIM_CR1_OFFSET)

int iowrite_example() __attribute__((irqbox));
int iowrite_example()
{
    rCR1 = 0x0;
}