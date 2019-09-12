#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define MAXLINE 1024
#define _add "./myadd"

/*
    信号捕获函数
*/
static void
sig_pipe(int signo)
{
    printf("SIGPIPE cuaght \n");
    exit(1);
}

/*
    popen：只提供连接到另一个进程的标准输入或标准输出的一个单向管道
    协同进程：连接到另一个进程的两个单向管道：一个连接到其标准输入，另一个则来自其标准输出，我们将数据写到其标准输入，处理后，再从其标准输出读取数据
*/
int 
main(int argc, char *argv[]) 
{
    int n, fd1[2], fd2[2];
    pid_t pid;
    char line[MAXLINE];

    // 定义SIGPIPE的处理函数
    if (signal(SIGPIPE, sig_pipe) == SIG_ERR)
        perror("signal error \n");
    // 创建两个管道
    if (pipe(fd1) < 0 || pipe(fd2) < 0)
        perror("pipe error \n");

    if ((pid = fork()) < 0)
        perror("fork error \n");
    // 父进程
    else if (pid > 0)
    {
        // 关闭本端第一个管道的读，写到子进程的stdin
        close(fd1[0]);
        // 关闭本端第二个管道的写，读取子进程的stdout
        close(fd2[1]);

        // 父进程从标准输入读取数据
        while(fgets(line, MAXLINE, stdin) != NULL)
        {
            n = strlen(line);

            // 读取的数据，被子进程读出来，即写到子进程的标准输入
            if (write(fd1[1], line, n) != n)
                perror("write pipe error \n");
            // 从子进程的标准输出读取数据
            if ((n = read(fd2[0], line, MAXLINE)) < 0)
                perror("read pipe error \n");
            
            if (n == 0)
            {
                perror("child closed pipe \n");
                break;
            }

            line[n] = 0;

            // 从子进程的标准输出读取的数据，打印到父进程的标准输出
            if (fputs(line, stdout) == EOF)
                perror("fputs error \n");
        }

        if (ferror(stdin))
            perror("fgets error on stdin \n");

        exit(0);
    }
    // 子进程
    else
    {
        // 关闭本端第一个管道的写，读取/输入的数据，从父进程写入
        close(fd1[1]);
        // 关闭本端第二个管道的读，写入/输出的数据，被父进程读取
        close(fd2[0]);

        // 子进程：本端第一个管道的读 fd1[0]连接到标准输入
        if (fd1[0] != STDIN_FILENO)
        {
            if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
                perror("dup2 error to stdin \n");
            
            close(fd1[0]);
        }

        // 子进程：本端第二个管道的写 fd2[1]连接到标准输出
        if (fd2[1] != STDOUT_FILENO)
        {
            if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
                perror("dup2 error to stdout \n");
            
            close(fd2[1]);
        }
        
        // 执行命令程序
        if (execl(_add, "myadd", (char *)0) < 0)
            perror("execl error \n");
    }
    
    exit(0);
}

