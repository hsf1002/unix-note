#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

static void
sig_alrm(int signo)
{
    /* nothing to do, just wake up sigsuspend */
}

/*
    sleep的另一种实现：按照POSIX要求，可靠的处理信号，避免了竞争条件，但是仍未处理与以前设置的闹钟的交互作用，POSIX未显式的对这些交互定义
*/
void
sleep(unsigned int seconds)
{
    // 进程新的信号屏蔽字
    sigset_t newmask;
    // 进程原有的信号屏蔽字
    sigset_t oldmask;
    // 进程阻塞的信号屏蔽字
    sigset_t suspmask;
    // 进程新的处理方式
    struct sigaction newact;
    // 进程旧的处理方式
    struct sigaction oldact;
    unsigned int unslept;

    // 设置handler，保存信息
    newact.sa_handler = sig_alrm;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    // 信号SIGALRM的原有处理方式保存到oldact(终止进程)，设置新的处理方式为newact(信号处理函数)
    if (sigaction(SIGALRM, &newact, &oldact) < 0)
        return(-1);
    
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    // 将进程原有的信号屏蔽字保存到oldmask，并设置新的信号屏蔽字为newmask，即在原有屏蔽字基础上以阻塞方式添加信号SIGALRM
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        return(-1);

    // 设置定时开始
    alarm(seconds);
    suspmask = oldmask;

    // 确保suspmask信号集中，信号SIGALRM没有阻塞
    sigdelset(&suspmask, SIGALRM);

    // 阻塞在此，等待捕获任何信号
    sigsuspend(&suspmask);

    // 从sigsuspend退出，恢复原有的信号屏蔽字，即捕获到某种信号，SIGALRM被阻塞。获取闹钟剩余秒数
    unslept = alarm(0);

    // 恢复信号SIGALRM的原有处理方式(终止进程)
    if (sigaction(SIGALRM, &oldact, NULL) < 0)
        return(-1);

    // 恢复进程原有的信号屏蔽字，将不再阻塞信号SIGALRM
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        return(-1);
    
    return(unslept);
}
