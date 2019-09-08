#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAXLINE 1024
#define DEF_PAGER "/usr/bin/more"

/*
    分页功能的程序
*/
int 
main(int argc, char *argv[]) 
{
    int n;
    int fd[2];
    pid_t pid;
    char *pager, *argv0;
    char line[MAXLINE];
    FILE *fp;

    if (argc != 2)
        perror("usage: a.out <pathname> \n");

    if ((fp = fopen(argv[1], "r")) == NULL)
        perror("open failed \n");
    
    if (pipe(fd) < 0)
        perror("pipe error \n");

    if ((pid = fork()) < 0)
        perror("fork error \n");

    // 父进程
    else if (pid > 0)
    {
        // 关闭读端
        close(fd[0]);

        // 从传入的文件读取数据
        while(fgets(line, MAXLINE, fp) != NULL)
        {
            n = strlen(line);
            // 读出来的数据写到写端，即子进程
            if (write(fd[1], line, n) != n)
                perror("write error to pipe \n");
        }

        if (ferror(fp))
            perror("fgets error \n");

        // 写完之后，关闭写端
        close(fd[1]);

        if (waitpid(pid, NULL, 0) < 0)
            perror("waitepid error \n");
        
        exit(0);
    }
    // 子进程
    else
    {
        // 关闭写端
        close(fd[1]);

        // 读端如果不是标准输入
        if (fd[0] != STDIN_FILENO)
        {
            // 使得标准输入变成读端
            if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
                perror("dup2 error to stdin \n");
            // 关闭读端
            close(fd[0]);
        }

        // 获取环境变量失败，赋予默认值
        if ((pager = getenv("PAGER")) == NULL)
            pager = DEF_PAGER;
        // 取pager的名字
        if ((argv0 = strrchr(pager, '/')) != NULL)
            argv0++;
        else
            argv0 = pager;
        printf("pager = %s, argv0 = %s \n", pager, argv0);
        
        // 执行分页程序
        if (execl(pager, argv0, (char *)0) < 0)
            perror("execl error \n");
    }
    
    exit(0);
}


