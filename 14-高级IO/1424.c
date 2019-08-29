#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

/*
    从文件描述符读取n个字节
*/
ssize_t 
readn(int fd, void *ptr, size_t n)
{
    size_t nleft;
    ssize_t nread;

    nleft = n;

    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            // 出错，直接返回
            if (nleft == n)
                return -1;
            // 出错，返回已经读到的数量 
            else 
                break;
        }
        // 出错，EOF
        else if (nread == 0)
        {
            break;
        }

        nleft -= nread;
        ptr += nread;
    }
    
    return (n - nleft);

}

/*
    向文件描述符写入n个字节
*/
ssize_t 
writen(int fd, const void *ptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;

    nleft = n;

    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) < 0)
        {
            // 出错，直接返回
            if (nleft == n)
                return -1;
            // 出错，返回已经写入的数量 
            else 
                break;
        }
        // 出错，EOF
        else if (nwritten == 0)
        {
            break;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    
    return (n - nleft);
}

/*
    使用readn和writen读写数据
*/
int 
main(int argc, char *argv[]) 
{
    int ifd, ofd, n;
    char buf[BUFSIZ];

    // 打开输入文件
    if ((ifd = open("/etc/passwd", O_RDONLY)) < 0)
    {
        perror("open error \n");
    }
    else
    {
        n = readn(ifd, buf, BUFSIZ);
        printf("readn n = %d \n", n);

        if (n > 0)
        {
            // 创建输出文件
            if ((ofd = open("wn.txt", O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
            {
                perror("create error \n");
            }
            else
            {
                n = writen(ofd, buf, BUFSIZ);
                printf("writen n = %d \n", n);
            }            
        }
    }

    exit(0);
}


