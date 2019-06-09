#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static jmp_buf jmpbuffer;
static int globval;

static void f1(int, int, int, int);
static void f2(void);


/*
    longjmp对于自动变量、全局变量、寄存器变量、静态变量和易失变量的影响

cc 713.c // 不进行任何优化的编译 
./a.out 
in fun1():
globval = 95, autoval = 96, regival = 97, volaval = 98, statval = 99 
after longjmp: 
globval = 95, autoval = 96, regival = 97, volaval = 98, statval = 99

gcc -O 713.c // 进行全部优化的编译
./a.out
in fun1():
globval = 95, autoval = 96, regival = 97, volaval = 98, statval = 99
after longjmp:
globval = 95, autoval = 2, regival = 3, volaval = 98, statval = 99

1. 全局bl、易失变量和静态变量不受优化的影响
2. 不进行优化，5个变量都放在存储器中，优化之后，auto和register变量存放在了寄存器中
*/
int main(int argc, char *argv[])
{
    int autoval;
    register int regival;
    volatile int volaval;
    static int statval;

    globval = 1;
    autoval = 2;
    regival = 3;
    volaval = 4;
    statval = 5;

    if (setjmp(jmpbuffer) != 0)
    {
        printf("after longjmp: \n");
        printf("globval = %d, autoval = %d, regival = %d, volaval = %d, statval = %d \n", \
            globval, autoval, regival, volaval, statval);
        exit(0);
    }

    globval = 95;
    autoval = 96;
    regival = 97;
    volaval = 98;
    statval = 99;

    f1(autoval, regival, volaval, statval);

    return(0);
}

static void f1(int i, int j, int k, int l)
{
    printf("in fun1():\n");
    printf("globval = %d, autoval = %d, regival = %d, volaval = %d, statval = %d \n", \
            globval, i, j, k, l);
    f2();
}

static void f2(void)
{
    longjmp(jmpbuffer, 1);
}
