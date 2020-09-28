#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

#define MAXLINE 100

static jmp_buf env_alrm;


static void
sig_alarm(int signo)
{
    longjmp(env_alrm, 1);
}

/*
    带时间限制调用read，使用longjmp

./a.out 
hello world read timeout 
: Success
ddddd
hello world ddddd

    不管系统是否重新启动被中断的系统调用，程序都会如预期工作，但是仍然存在10-8与其他信号处理程序交互的问题

    如果要对IO操作设置时间限制，可以使用longjmp

    另一种选择是使用select或poll
*/
int main(void)
{
    int n;
    char line[MAXLINE];

    if (signal(SIGALRM, sig_alarm) == SIG_ERR)
        perror("signal(SIGALRM) error \n");

	// 10秒内没有任何操作，则又会重复
    if (setjmp(env_alrm) != 0)
        perror("read timeout \n");

    // 设置10秒的定时
    alarm(10);
    
    // 10秒内如果按回车，则会读取到数据，read退出
	if ((n = read(STDIN_FILENO, line, MAXLINE)) < 0)
        perror("read error \n");

    // 关闭定时
    alarm(0);

    write(STDOUT_FILENO, line, n);
    exit(0);
}
