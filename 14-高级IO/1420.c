#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BSZ 4096

unsigned char buf[BSZ];
static int oofd = 0;

unsigned char 
translate(unsigned char c)
{
    if (isalpha(c))
    {
        if (c >= 'n')
            c -= 13;
        else if (c >= 'a')
            c += 13;
        else if (c >= 'N')
            c -= 13;
        else
            c += 13;
    }

    return c;
}

/*
    ROT-13算法
*/
int 
main(int argc, char *argv[]) 
{
    int ifd, ofd, i, n, nw;
    pid_t pid;
    char buf[5];
    struct stat statbuf;

    if (argc != 3)
    {
        fprintf(stderr, "usage: rot-13 infile outfile \n");
        exit(1);
    }

    // 打开输入文件
    if ((ifd = open(argv[1], O_RDONLY)) < 0)
    {
        perror("open error \n");
    }

    // 创建输出文件
    if ((oofd = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
    {
        perror("create error \n");
    }

    printf("oofd = %d\n", oofd);
while ((n = read(ifd, buf, BSZ)) > 0)
    {
        for (i=0; i<n; i++)
        {
            buf[i] = translate(buf[i]);
        }

    printf("oofd = %d\n", oofd);
        if ((nw = write(oofd, buf, n)) != n)
        {
        printf("oofd = %d, n = %d, nw = %d \n", oofd, n, nw);
            if (nw < 0)
            {
                perror("write failed \n");
            }
            else
            {
                fprintf(stderr, "short write (%d/%d) \n", nw, n);
            }
        }
    }

    fsync(ofd);
    
    exit(0);
}


