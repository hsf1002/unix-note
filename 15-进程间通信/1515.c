#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define MAXLINE 1024
#define _2lower "./my2lower"

/*
    使用popen调用一个将字母转小写的程序
*/
int 
main(int argc, char *argv[]) 
{
    char line[MAXLINE];
    FILE *fpin;

    // popen会fork一个子进程，调用exce执行_2lower，文件指针fpin会连接到该子进程的标准输出
    if ((fpin = popen(_2lower, "r")) == NULL)
        perror("popen failed \n");

    for (;;)
    {
        fputs("prompt> ", stdout);
        fflush(stdout);

        // 从管道读取数据
        if (fgets(line, MAXLINE, fpin) == NULL)
            break;

        // 读出来的数据写到标准输出
        if (fputs(line, stdout) == EOF)
            perror("fputs error to pipe \n");
    }
    
    if (pclose(fpin) == -1)
        perror("pclose error \n");

    putchar('\n');
    
    exit(0);
}

