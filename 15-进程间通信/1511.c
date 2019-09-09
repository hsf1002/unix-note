#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAXLINE 1024
#define PAGER "${PAGER:-more}"

/*
    使用popen实现分页功能的程序
*/
int 
main(int argc, char *argv[]) 
{
    char line[MAXLINE];
    FILE *fpin;
    FILE *fpout;

    if (argc != 2)
        perror("usage: a.out <pathname> \n");

    if ((fpin = fopen(argv[1], "r")) == NULL)
        perror("fopen failed \n");

    // 将fpout以写的方式连接到PAGER
    if ((fpout = popen(PAGER, "w")) == NULL)
        perror("popen failed \n");

    // 从传入的文件读取数据
    while(fgets(line, MAXLINE, fpin) != NULL)
    {
        // 读出来的数据写到PAGER去
        if (fputs(line, fpout) == EOF)
            perror("fputs error to pipe \n");
    }

    if (ferror(fpin))
        perror("fgets error \n");
    
    if (pclose(fpout) == -1)
        perror("pclose error \n");
    
    exit(0);
}

