#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/*
    abort的一种实现
*/
void
abort(void)
{
    sigset_t mask;
    struct sigaction action;

    // 给信号SIGABRT注册信号处理函数
    sigaction(SIGABRT, NULL, &action);

    // 如果处理方式是忽略，设置为默认（终止进程）
    if (action.sa_handler == SIG_IGN)
    {
        action.sa_handler = SIG_DFL;
        sigaction(SIGABRT, &action, NULL);
    }

    // 设置处理方式为默认后，立即刷新所有输入输出流
    if (action.sa_handler == SIG_DFL)
        fflush(NULL);
    
    // 初始化阻塞信号集
    sigfillset(&mask);
    // 确保SIGABRT不被阻塞
    sigdelset(&mask, SIGABRT);
    // 设置信号屏蔽字
    sigprocmask(SIG_SETMASK, &mask, NULL);
    // 向进程发送SIGABRT信号
    kill(getpid(), SIGABRT);

    // 如果走到这里，处理SIGABRT并返回
    fflush(NULL);
    // 设置处理方式为默认
    action.sa_handler = SIG_DFL;
    sigaction(SIGABRT, &action, NULL);
    // 以防万一
    sigprocmask(SIG_SETMASK, &mask, NULL);
    // 向进程再次发送SIGABRT信号
    kill(getpid(), SIGABRT);
    // 永远不应该走到这
    exit(1);
}
