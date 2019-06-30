#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static volatile sig_atomic_t sigflag;

static sigset_t newmask;
static sigset_t oldmask;
static sigset_t zeromask;

/*
    suspend的第三种作用：实现父进程、子进程之间的同步
*/

/*
    one sig handler for SIGUSR1+SIGUSR2
*/
static void
sig_usr(int signo)
{
    sigflag = 1;
}

void
TELL_WAIT(void)
{
    // 注册信号处理函数，SIGUSR1由父进程发给子进程
    if (signal(SIGUSR1, sig_usr) == SIG_ERR)
        ferror("signal(SIGUSR1) error");
    // 注册信号处理函数，SIGUSR2由子进程发给父进程
    if (signal(SIGUSR2, sig_usr) == SIG_ERR)
        ferror("signal(SIGUSR2) error");

    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);

    // 保存原有的信号屏蔽字，设置新的信号屏蔽字，即将SIGUSR1+SIGUSR2阻塞添加
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        ferror("SIG_BLOCK error");
}

void 
TELL_PARENT(pid_t pid)
{
    // 告知父进程我已结束
    kill(pid, SIGUSR2);
}

void
WAIT_PARENT(void)
{
    // 收到SIGUSR1，即父进程结束，退出，并将信号屏蔽字恢复为newmask
    while (sigflag == 0)
        sigsuspend(&zeromask);  // 等待父进程结束

    sigflag = 0;

    // 恢复原来的信号屏蔽字
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        ferror("SIG_SETMASK error");
}

void 
TELL_CHILD(pid_t pid)
{
    // 告知子进程我已结束
    kill(pid, SIGUSR1);
}

void
WAIT_CHILD(void)
{
    // 收到SIGUSR2，即子进程结束，退出，并将信号屏蔽字恢复为newmask
    while (sigflag == 0)
        sigsuspend(&zeromask);  // 等待子进程结束

    sigflag = 0;

    // 恢复原来的信号屏蔽字
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        ferror("SIG_SETMASK error");
}
