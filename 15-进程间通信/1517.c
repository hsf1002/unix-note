#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#define MAXLINE 1024


/*
    将两个数相加的程序
*/
#if 0
int
main(void)
{
    int n, int1, int2;
    char line[MAXLINE];

    while ((n = read(STDIN_FILENO, line, MAXLINE)) > 0)
    {
        line[n] = 0;

        // 将line的数据以两个整数格式放入int1和int2
        if (sscanf(line, "%d%d", &int1, &int2) == 2)
        {
            // 将int1和int2相加后的结果放入line
            sprintf(line, "%d\n", int1 + int2);

            n = strlen(line);

            if (write(STDOUT_FILENO, line, n) != n)
                perror("write error \n");
        }
        else
        {
            if (write(STDOUT_FILENO, "invalid args\n", 13) != 13)
                perror("write error \n");
        }
    }

    exit(0);
}
#else
int
main(void)
{
    int n, int1, int2;
    char line[MAXLINE];

    // 将标准输入设置为行缓冲，当fgets可用时，就返回
    if (setvbuf(stdin, NULL, _IOLBF, 0) != 0)
        perror("setvbuf error \n");
    // 将标准输出设置为行缓冲，当printf遇到换行时，执行fflush操作
    if (setvbuf(stdout, NULL, _IOLBF, 0) != 0)
        perror("setvbuf error \n");

    // 因为标准输入对应的是一个管道，所以标准IO库默认是全缓冲，标准输出也是如此
    // 当myadd从标准输入读取发生阻塞时，主程序从管道读取时也会发生阻塞，于是产生死锁
    // 上述代码将标准输入和标准输出设置为行缓冲，可避免此问题
    while (fgets(line, MAXLINE, stdin) > 0)
    {
        // 将line的数据以两个整数格式放入int1和int2
        if (sscanf(line, "%d%d", &int1, &int2) == 2)
        {
            // 将int1和int2相加后的结果放入line
            if (printf("%d\n", int1 + int2) == EOF)
                perror("printf error \n");
        }
        else
        {
            if (printf("invalid args\n") == EOF)
                perror("printf error \n");
        }
    }

    exit(0);
}   
#endif
