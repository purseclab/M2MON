#include <stdio.h>
//void foo(){};
//int global;
//struct a {
//int a;
//int b;
//};
int testup_svcal() __attribute__((irqbox));
//void fun(int a)
//{
//}
//static void inline write()
//{
//    *(volatile unsigned int *) 0xFFE1000 = 0x0;
//}
int testup_svcal()
{
//    int a = 0;
//    a = 2;
//    *(volatile unsigned int *) 0xFFE1000 = 0x0;
//    a = *(volatile unsigned int *) 0xFFE1000;
//
//    *(volatile unsigned char *) 0xFFE1000 = 0x0;
//    a = *(volatile unsigned char *) 0xFFE1000;
//
//    struct a st;
//    st.a = 0;
//
//    struct a * pst = &st;
//
//    pst->a = 0;
//
//
//    volatile unsigned int * mem_ref = (volatile unsigned int *)0xFFE1000;
//
//    volatile unsigned int * mem_ref2 = mem_ref;

//    *mem_ref = 0x0;
//    int * simple = &a;

//    *simple = 5;
  
    int b = 0;
    int * shouldNot = &b;
    *shouldNot = 10;


//    *(volatile unsigned int *) 0xEEE1000 = 0x0;

//    write();
//
//    global = 2;
//    foo();

//    // fun_ptr is a pointer to function fun()
//    void (*fun_ptr)(int) = &fun;
//
//    /* The above line is equivalent of following two
//       void (*fun_ptr)(int);
//       fun_ptr = &fun;
//    */
//
//    // Invoking fun() using fun_ptr
//    (*fun_ptr)(10);
//    fun(10);

    return 0;
}
//int test2(int c)
//{
//    return 1;
//}
//void test3(int d)
//{
//    testup_svcal();
//}
int main()
{
//    printf("Crap");
//    int c = 100;
//    int b = test2(c);
//    b++;
//    test3(b);
//    testup_svcal();
//    testup_svcal();
    return 0;
}
