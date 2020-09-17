#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>


jmp_buf jmpbuffer;

int fun1();
int fun2();
int fun3();
int fun4();

/*
     setjmp和longjmp：跳过栈帧

first setjmp success 
fun4 called start 
second setjmp success

*/
int main(int argc, char *argv[])
{
    // 第一次总是返回0，以后的返回值则是longjmp的第二个参数
	int result = setjmp(jmpbuffer);

    if (result == 1)
    {
        printf("second setjmp success \n");
        exit(0);
    }
	else if (result == 0)
	{
		printf("first setjmp success \n");
	}
	else
	{
		perror("setjmp error \n");
		exit(0);
	}
    fun1();
    printf("main done \n");

    exit(0);
}


int fun1()
{
    fun2();

    printf("fun1 called \n");

    return 0;
}

int fun2()
{
    fun3();

    printf("fun2 called \n");

    return 0;
}

int fun3()
{
    fun4();

    printf("fun3 called \n");

    return 0;
}

int fun4()
{
    printf("fun4 called start \n");

    longjmp(jmpbuffer, 1);

    printf("fun4 called end \n");

    return 0;
}
