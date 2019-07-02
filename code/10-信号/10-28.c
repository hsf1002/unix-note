#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/*
    system的另一种实现：按照POSIX要求，忽略SIGINT和SIGQUIT，阻塞SIGCHLD
*/
void
system(const char * cmdstring)
{
    pid_t pid;
    int status;
    // 进程新的信号屏蔽字
    sigset_t childmask;
    // 进程原有的信号屏蔽字
    sigset_t savemask;
    // 忽略的信号处理方式
    struct sigaction ignore;
    // 信号SIGINT原有的处理方式
    struct sigaction saveintr;
    // 信号SIGQUIT原有的处理方式
    struct sigaction savequit;

    if (cmdstring == NULL)
        return(1);
    // 忽略SIGINT和SIGQUIT
    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    ignore.sa_flags = 0;

    // 信号SIGINT的原有处理方式保存到saveintr，设置新的处理方式为ignore，即忽略
    if (sigaction(SIGINT, &ignore, &saveintr) < 0)
        return(-1);
    // 信号SIGQUIT的原有处理方式保存到savequit，设置新的处理方式为ignore，即忽略    
    if (sigaction(SIGQUIT, &ignore, &savequit) < 0)
        return(-1);
    
    sigemptyset(&childmask);
    sigaddset(&childmask, SIGCHLD);
    // 将进程原有的信号屏蔽字保存到savemask，并设置新的信号屏蔽字为childmask，即在原有屏蔽字基础上以阻塞方式添加信号SIGCHLD
    if (sigprocmask(SIG_BLOCK, &childmask, &savemask) < 0)
        return(-1);
    
    // 创建子进程失败
    if ((pid = fork()) < 0)
        status = -1;
    else if (pid == 0)
    {
        // 在子进程中，首先，恢复原有的信号屏蔽字和信号处理方式
        sigaction(SIGINT, &saveintr, NULL);
        sigaction(SIGQUIT, &savequit, NULL);
        sigprocmask(SIG_SETMASK, &savemask, NULL);
        // 执行命令
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        // 正常退出
        _exit(127);
    }
    else
    {
        // 在父进程中，等待获取子进程状态
        while (waitpid(pid, &status, 0) < 0)
            if (errno != EINTR)
            {
                status = -1;
                break;
            }
    }

    // 恢复信号SIGINT原有的处理方式
    if (sigaction(SIGINT, &saveintr, NULL) < 0)
        return(-1);
    // 恢复信号SIGQUIT原有的处理方式    
    if (sigaction(SIGQUIT, &savequit, NULL) < 0)
        return(-1);
    // 恢复进程原有的信号屏蔽字    
    if (sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
        return(-1);
    
    return(status);
}
