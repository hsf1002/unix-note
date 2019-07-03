#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>


static void pr_mask(const char *str);

static void
sig_init(int signo)
{
    // 打印sig_init中的信号屏蔽字
    pr_mask("in sig_init: ");
}

/*
    提供了 保护代码临界区，使其不被特定信号中断的正确方法: 

./a.out 
main start: 
in critical region:  SIGINT
^Cin sig_init:  SIGINT SIGUSR1
after return from sissuspend:  SIGINT
main end:
*/
int 
main(void)
{
    // 原来的信号屏蔽字+SIGINT
    sigset_t newmask;
    // 原来的信号屏蔽字
    sigset_t oldmask;
    // 原来的信号屏蔽字+SIGUSR1
    sigset_t waitmask;

    // 打印主程序开始时的信号屏蔽字
    pr_mask("main start: ");

    // 注册SIGINT的信号处理函数
    if (signal(SIGINT, sig_init) == SIG_ERR)
        ferror("signal(SIGINT) error");

    sigemptyset(&waitmask);
    sigaddset(&waitmask, SIGUSR1);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    // 旧的信号屏蔽字保存在oldmask，且设置新的屏蔽字，即在原有屏蔽字基础上，将信号SIGINT阻塞添加
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        perror("SIG_BLOCK error");
    
    /*
        代码临界区
    */
    // 打印sissuspend之前的信号屏蔽字
    pr_mask("in critical region: ");

    // 暂停，阻塞在此处，将信号屏蔽字设置为SIGUSR1，要么捕捉到SIGUSR1，要么发生了一个使进程终止的信号，才会返回
    // 返回之后，将信号屏蔽字设置为newmask
    if (sigsuspend(&waitmask) != -1)    // 无成功返回值，总是返回-1
        ferror("sissuspend error");

    // 打印sissuspend返回时的信号屏蔽字
    pr_mask("after return from sissuspend: ");

    // 恢复旧的信号屏蔽字，即将信号屏蔽字设置为oldmask，信号SIGINT不再阻塞
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        perror("SIG_SETMASK error");

    // 打印主程序结束时的信号屏蔽字
    pr_mask("main end: ");

    exit(0);
}


/*
    供外部调用: 检测进程的信号屏蔽字都包含哪些信号
*/
static void
pr_mask(const char *str)
{
    sigset_t sigset;
    int errno_save;

    // 保存错误码
    errno_save = errno;

    // 获取进程的信号屏蔽字
    if (sigprocmask(0, NULL, &sigset) < 0)
    {
        perror("sigprocmask error \n");
    }
    // 只列出了常见的几种信号，不能检测到SIGKILL或SIGSTOP
    else
    {
        printf("%s", str);
        // 信号屏蔽字中是否包含SIGINT
        if (sigismember(&sigset, SIGINT))
            printf(" SIGINT");
        if (sigismember(&sigset, SIGQUIT))
            printf(" SIGQUIT");
        if (sigismember(&sigset, SIGALRM))
            printf(" SIGALRM");
        if (sigismember(&sigset, SIGABRT))
            printf(" SIGABRT");
        if (sigismember(&sigset, SIGFPE))
            printf(" SIGFPE");
        if (sigismember(&sigset, SIGSEGV))
            printf(" SIGSEGV");
        if (sigismember(&sigset, SIGPIPE))
            printf(" SIGPIPE");
        if (sigismember(&sigset, SIGSYS))
            printf(" SIGSYS");
        if (sigismember(&sigset, SIGTERM))
            printf(" SIGTERM");
        if (sigismember(&sigset, SIGCHLD))
            printf(" SIGCHLD");
        if (sigismember(&sigset, SIGUSR1))
            printf(" SIGUSR1");
        if (sigismember(&sigset, SIGUSR2))
            printf(" SIGUSR2");

        printf("\n");
    }
    
    // 恢复错误码
    errno = errno_save;
}
