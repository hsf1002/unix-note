#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>


/*
    供外部调用: 检测进程的信号屏蔽字都包含哪些信号
*/
void
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
