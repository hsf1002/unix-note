#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>


static sigjmp_buf jmpbuf;
/*
    volatile保证变量在三个线程main、sig_usr1和sig_alrm的同步
    sig_atomic_t保证原子操作，写操作时不被中断
*/
static volatile sig_atomic_t canjump; 

static void pr_mask(const char *str);

static void
sig_usr1(int signo)
{
    time_t starttime;

    // sigsetjmp没有设置成功，不可以进行跳转
    if (canjump == 0)
        return;
    
    // 打印sig_usr1开始时的信号屏蔽字
    pr_mask("starting sig_usr1: ");

    // 3秒定时开始
    alarm(3);

    // 开始时间
    starttime = time(NULL);

    for (;;)
        // 阻塞在此，5秒后退出阻塞
        if (time(NULL) > starttime + 5)
            break;

    // 打印sig_usr1结束时的信号屏蔽字
    pr_mask("finishing sig_usr1: ");

    // 重新设置为不可跳转
    canjump = 0;

    // 即将跳转到主程序
    siglongjmp(jmpbuf, 1);
}

static void
sig_alrm(int signo)
{
    // 打印sig_alrm中的信号屏蔽字
    pr_mask("in sig_alrm: ");
}

/*
    提供了一种保护机制：使得在jmpbuf（跳转缓冲）尚未由sigsetjmp初始化时，防止调用信号处理函数

    main: 信号屏蔽字是0，没有任何信号阻塞
    sig_usr1: 信号屏蔽字是SIGUSR1
    sig_alrm: 信号屏蔽字是SIGUSR1 | SIGALRM，当调用一个信号处理程序时，被捕捉到的信号加到进程的当前信号屏蔽字中，从信号处理程序返回时，恢复为原来的屏蔽字
    另外，siglongjmp恢复了sigsetjmp所保存的信号屏蔽字

./a.out &
[2] 78133
skydeiMac:10-信号 sky$ starting main:

skydeiMac:10-信号 sky$ kill -USR1 78133
starting sig_usr1:  SIGUSR1
skydeiMac:10-信号 sky$ in sig_alrm:  SIGALRM SIGUSR1
finishing sig_usr1:  SIGUSR1
ending main:

[2]-  Done                    ./a.out

*/
int 
main(void)
{
    // 注册SIGUSR1的信号处理函数
    if (signal(SIGUSR1, sig_usr1) == SIG_ERR)
        ferror("signal(SIGUSR1) error");
    // 注册SIGALRM的信号处理函数
    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        ferror("signal(SIGALRM) error");

    // 打印主程序开始时的信号屏蔽字
    pr_mask("starting main: ");

    // 设置SIGUSR1信号处理函数的返回地址
    if (sigsetjmp(jmpbuf, 1))
    {
        // 打印主程序结束时的信号屏蔽字
        pr_mask("ending main: ");

        exit(0);
    }

    // sigsetjmp已经设置成功，可以进行跳转啦
    canjump = 1;

    // 主程序阻塞在此，除非捕获到信号
    for (;;)
    {
        pause();
    }
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
