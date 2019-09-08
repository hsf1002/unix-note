#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAXLINE 1024

/*
    创建一个从父进程到子进程的管道，且父进程经由该管道向子进程传送数据
*/
int 
main(int argc, char *argv[]) 
{
    int n;
    int fd[2];
    pid_t pid;
    char line[MAXLINE];

    if (pipe(fd) < 0)
        perror("create pipe error \n");
    
    if ((pid = fork()) < 0)
        perror("fork error \n");
    // 父进程
    else if (pid > 0)
    {
        // 关闭读端
        close(fd[0]);
        write(fd[1], "hello world\n", 12);
    }
    // 子进程
    else
    {
        // 关闭写端
        close(fd[1]);
        n = read(fd[0], line, MAXLINE);
        write(STDOUT_FILENO, line, n);
    }
    
    exit(0);
}


