#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

typedef void (*Sigfunc)(int signo); 

/*
    signal的另一种实现，力图阻止被中断的系统调用重启动
    macOS没有定义SA_INTERRUPT
*/
Sigfunc * 
signal(int signo, Sigfunc *func)
{
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT; // 由此信号中断的系统调用不自动重启 
#endif
    // 保存旧的动作到oact，并设置新的动作
    if (sigaction(signo, &act, &oact) < 0)
    {
        return SIG_ERR;
    }
    
    return oact.sa_handler;
}
