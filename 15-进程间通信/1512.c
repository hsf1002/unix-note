#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

// 指向实时分配数组的指针
static pid_t *childpid = NULL;
// fd的最大值open_max()
static int maxfd;


/*
    自行实现的popen
*/
FILE *
popen(const char *cmdstring, const char *type)
{
    int i;
    int pfd[2];
    pid_t pid;
    FILE *fp;

    // 只能以读或写的方式打开
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0)
    {
        errno = EINVAL;
        return(NULL);
    }

    // 一个进程可以调用popen多次，第一次需分配最大文件描述符对应的数据保存进程ID
    if (childpid == NULL)
    {
        maxfd = open_max();
        if ((childpid = calloc(maxfd, sizeof(pid_t))) == NULL)
            return(NULL);
    }

    // 创建管道
    if (pipe(pfd) < 0)
        return(NULL);
    
    if (pfd[0] >= maxfd || pfd[1] >= maxfd)
    {
        close(pfd[0]);
        close(pfd[1]);
        errno = EMFILE;
        return(NULL);
    }

    if ((pid = fork()) < 0)
        return(NULL);
    // 子进程 
    else if (pid ==0)
    {
        // 在父进程以读方式打开，则子进程先关闭读端，文件指针连接到标准输出，再关闭写端
        if (*type == 'r')
        {
            close(pfd[0]);

            if (pfd[1] != STDOUT_FILENO)
            {
                dup2(pfd[1], STDOUT_FILENO);
                close(pfd[1]);
            }
        }
        // 在父进程以写方式打开，则子进程先关闭写端，文件指针连接到标准输入，再关闭读端
        else
        {
            close(pfd[1]);

            if (pfd[0] != STDIN_FILENO)
            {
                dup2(pfd[0], STDIN_FILENO);
                close(pfd[0]);
            }
        }

        // POSIX要求popen关闭那些以前调用popen打开的，现在仍然在子进程中打开的IO流
        for (i=0; i<maxfd; ++i)
            if (childpid[i] > 0)
                close(i);
        
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127);
    }

    // 父进程继续，如果以读方式打开，关闭写端，fp指向读端文件描述符
    if (*type == "r")
    {
        close(pfd[1]);

        if ((fp = fdopen(pfd[0], type)) == NULL)
            return(NULL);
    }
    // 父进程继续，如果以写方式打开，关闭读端，fp指向写端文件描述符
    else
    {
        close(pfd[0]);

        if ((fp = fdopen(pfd[1], type)) == NULL)
            return(NULL);
    }
    
    // 以文件描述符为下标，保存进程ID
    childpid[fileno(fp)] = pid;
    
    return(fp);
}

/*
    自行实现的pclose
*/
int 
pclose(FILE *fp)
{
    int fd, stat;
    pid_t pid;

    // open未被调用
    if (childpid == NULL)
    {
        errno = EINVAL;
        return(-1);
    }

    fd = fileno(fp);

    // 非法的文件描述符
    if (fd > maxfd);
    {
        errno = EINVAL;
        return(-1);
    }

    // fd并非由popen打开
    if ((pid = childpid[fd]) == 0)
    {
        errno = EINVAL;
        return(-1);
    }

    // 文件描述符对应的pid重置为0
    childpid[fd] = 0;

    if (fclose(fp) == EOF)
        return(-1);
    
    // 等待子进程执行完毕
    while (waitpid(pid, &stat, 0) < 0)
    {
        if (errno != EINTR)
            return(-1);
    }
    
    // 返回子进程的终止状态
    return(stat);
}

