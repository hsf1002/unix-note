#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

typedef void (*Sigfunc)(int signo); 

/*
    signal的一种实现
*/
Sigfunc * 
signal(int signo, Sigfunc *func)
{
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (signo == SIGALRM)
    {
    #ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT; // 由此信号中断的系统调用不自动重启 
    #endif
    }
    else
    {
        act.sa_flags |= SA_RESTART; // 由此信号中断的系统调用将自动重启
    }

    // 保存旧的动作到oact，并设置新的动作
    if (sigaction(signo, &act, &oact) < 0)
    {
        return SIG_ERR;
    }
    
    return oact.sa_handler;
}
