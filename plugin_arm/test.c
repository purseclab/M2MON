#include <stdio.h>
#if 0
void foo(){};
int global;
struct a {
int a;
int b;
};
int testup_svcal() __attribute__((irqbox));
static inline void write()
{
    *(volatile unsigned int *) 0xFFE1000 = 0x0;
}
#define HRT_TIMER_BASE       0x40012c00
#define REG(_reg)             (*(volatile unsigned *)(HRT_TIMER_BASE + _reg))
#define STM32_GTIM_CR1_OFFSET 0x0000
#define rCR1                  REG(STM32_GTIM_CR1_OFFSET)

int getreg32(int * a)
{
   return 0;
}

extern int j;
int nest_call() __attribute__((irqbox));
#endif
//int nest_call()
//{
 //   int j = j +32;
//}
extern int i;
unsigned int fish;
unsigned int bazooka;
extern int cputreg32(unsigned int a,unsigned int b);
typedef struct address 
{ 
   unsigned long  DR;
   unsigned long  DR_1; 
   unsigned long  DR_2;
} UART;
volatile unsigned long * ppg= 999999;
// externed global variables don't work
UART * uart_1 = 0xFE110000;
int iowrite_example() __attribute__((irqbox_test));
int iowrite_example()
{
//	int p0;int *a = &p0;*a = 0;
//nest_call();

//	*(volatile unsigned int *) 0xFFE1000 = 0x0;
#if 0
        int offset = 0x10;
        int addr = (2*offset) + 0xFE1000;
	volatile unsigned int * mem_pt = (volatile unsigned int *) addr;
	*mem_pt =0x1;
#endif
	UART * uart_2 = 0xFE10000;
	uart_2->DR = 0x1;
	uart_1->DR = 0x1;
//	*ppg = 0x1;
#if 0
	*ppg = 0x1;
	int p0; int p1;
	int *pp;
	if (i == 9)
		pp = &p0;
	else 
		pp = &p1;
	*pp = 10;
        return p0;
#endif 
#if 0
	*mem_pt = 0x0;
#endif 
}
#if 0
int indirectref_example() __attribute__((irqbox));
int indirectref_example()
{
//    int p0;
//    int *a = &p0;
//    *a = 0; 
}

int testup_svcal()
{
#if 0
    int a = 0;
    a = 2;
    *(volatile unsigned int *) 0xFFE1000 = 0x0;
    a = *(volatile unsigned int *) 0xFFE1000;

    *(volatile unsigned char *) 0xFFE1000 = 0x0;
    a = *(volatile unsigned char *) 0xFFE1000;

    struct a st;
    st.a = 0;

    struct a * pst = &st;

    pst->a = 0;


    volatile unsigned int * mem_ref = 0xFFE1000;

    volatile unsigned int * mem_ref2 = mem_ref;

    *mem_ref = 0x0;
    unsigned int * simple = &a;

    *simple = 5;
  
    int b = 0;
    unsigned int * shouldNot = &b;
    *shouldNot = 10;


    *(volatile unsigned int *) 0xEEE1000 = 0x0;

//    write();

    global = 2;
//    foo();

    // fun_ptr is a pointer to function fun() 
    void (*fun_ptr)(int) = &fun;
  
    /* The above line is equivalent of following two
       void (*fun_ptr)(int);
       fun_ptr = &fun; 
    */
  
    // Invoking fun() using fun_ptr
    (*fun_ptr)(10);
    fun(10);
#endif 

}
int test2(int c)
{
    return 1;
}
void test3(int d)
{
    testup_svcal();
}
#endif

int main()
{
	iowrite_example();
    return 0;
}
