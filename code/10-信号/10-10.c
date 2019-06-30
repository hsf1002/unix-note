#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


#define MAXLINE 100


static void
sig_alarm(int signo)
{
    /* nothing to do, just return to interrupt the read */
}

/*
    带时间限制调用read
    
    alarm除了实现sleep之外，还可以对可能阻塞的操作设置时间上限值，但是以下代码有两个问题：
    1. 第一次调用alarm和read之间存在竞争条件，如果内核在两个函数之间使进程阻塞，且其时间长度超过闹钟时间，
        则read可能永远阻塞，可以将闹钟时间延长，但并不能消除竞争条件
    2. 如果系统调用时自动重启动的，则从SIGALRM信号处理程序返回时，read并不被中断，这种情况下设置时间限制不起作用
*/
int main(void)
{
    int n;
    char line[MAXLINE];

    if (signal(SIGALRM, sig_alarm) == SIG_ERR)
        perror("signal(SIGALRM) error \n");

    // 设置10秒的定时
    alarm(10);
    
    if ((n = read(STDIN_FILENO, line, MAXLINE)) < 0)
        perror("read error \n");

    // 关闭定时
    alarm(0);

    write(STDOUT_FILENO, line, n);
    exit(0);
}
