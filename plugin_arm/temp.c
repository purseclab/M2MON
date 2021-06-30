#define CR1_ADDR   0x40012c00
#define rCR1                  (*(volatile unsigned *)(CR1_ADDR))

int iowrite_example() __attribute__((irqbox));
int iowrite_example()
{
    rCR1 = 0;
    int i = rCR1;
}