#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>


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
    system的一种实现
*/
int my_system(const char * cmdstr)
{
    pid_t pid;
    int status;

    if (NULL == cmdstr)
    {
        return(1);
    }

    if ((pid = fork()) < 0)
    {
        status = -1;
    }
    else if(pid == 0)   
    {
        execl("/bin/sh", "sh", "-c", cmdstr, (char *)0);
        _exit(127); /* execl error */
    }
    else
    {
        while (waitpid(pid, &status, 0) < 0)
        {
            if (errno != EINTR)
            {
                status = -1;
                break;
            }
        }
    }

    return(status);
}

/*
./a.out
2019年 6月21日 星期五 06时45分30秒 CST
normal termination, exit status = 0
sh: nosuchcommand: command not found
normal termination, exit status = 127
sky      console  Apr 25 22:18
sky      ttys000  Apr 28 22:31
sky      ttys001  May  2 21:33
sky      ttys002  May 14 06:28
normal termination, exit status = 44

*/
int main(void)
{
    int status;

    if ((status = my_system("date")) < 0)
    {
        ferror("system() error");
    }

    pr_exit(status);

    if ((status = my_system("nosuchcommand")) < 0)
    {
        ferror("system() error");
    }

    pr_exit(status);

    if ((status = my_system("who; exit 44")) < 0)
    {
        ferror("system() error");
    }

    pr_exit(status);

    exit(0);
}
