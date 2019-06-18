#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
    退出的通用操作
*/
void pr_exit(int status)
{
    // 正常终止子进程
    if (WIFEXITED(status))
    {
        // 获取子进程返回信息
        printf("normal termination, exit status = %d \n", WEXITSTATUS(status));
    }
    // 异常终止子进程
    else if (WIFSIGNALED(status))
    {
        // 获取子进程终止的信号编号
        printf("abnormal termination, signal number = %d%s \n", WTERMSIG(status),
        // 是否有coredump产生
#ifdef WCOREDUMP
        WCOREDUMP(status) ? "(core file generated)" : "");
#else
        ");"
#endif    
    }
    // 暂停子进程
    else if(WIFSTOPPED(status))
    {
        // 获取使子进程暂停的信号编号
        printf("child stopped, signal number = %d \n", WSTOPSIG(status));
    }
}

/*
    exit的正常退出，abort的异常退出、coredump的产生

./a.out 
child exit 
normal termination, exit status = 7 
child abort 
abnormal termination, signal number = 6 
child coredump 
abnormal termination, signal number = 8

*/
int main(int argc, char **argv)
{
    int status;
    pid_t pid;

    // 第一次fork
    if ((pid = fork()) < 0)
    {
        ferror("fork error");
    }
    // 子进程正常退出
    else if(pid == 0)   /* child */
    {
        printf("child exit \n");
        exit(7);
    }
    if (wait(&status) != pid)
    {
        ferror("wait error");
    }

    pr_exit(status);

    // 第二次fork
    if ((pid = fork()) < 0)
    {
        ferror("fork error");
    }
    // 子进程异常退出
    else if(pid == 0)   /* child */
    {
        printf("child abort \n");
        abort();
    }
    if (wait(&status) != pid)
    {
        ferror("wait error");
    }

    pr_exit(status);

    // 第三次fork
    if ((pid = fork()) < 0)
    {
        ferror("fork error");
    }
    // 子进程产生coredump
    else if(pid == 0)   /* child */
    {
        printf("child coredump \n");
        status /= 0;
    }
    if (wait(&status) != pid)
    {
        ferror("wait error");
    }

    pr_exit(status);

    exit(0);
}
