#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

char buf[500000];



/**/
void
set_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
    {
        printf("fcntl F_GETFL error \n");
    }
    
    val |= flags;

    if (fcntl(fd, F_SETFL, val) < 0)
    {
        printf("fcntl F_SETFL error \n");
    }
}

/**/
void
clr_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
    {
        printf("fcntl F_GETFL error \n");
    }
    
    val &= ~flags;

    if (fcntl(fd, F_SETFL, val) < 0)
    {
        printf("fcntl F_SETFL error \n");
    }
}

/*
   非阻塞IO的一个例子，从标准输入读取，试图写到标准输出
*/
int main(int argc, char *argv[]) 
{
    int ntowrite, nwrite;
    char *ptr;

    // 从标准输入读取
    ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "read %d bytes \n", ntowrite);

    // 设置非阻塞状态
    set_fl(STDOUT_FILENO, O_NONBLOCK);

    ptr = buf;
    // 写到标准输出
    while (ntowrite > 0)
    {
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, ntowrite);
        fprintf(stderr, "nwrite = %d, errno = %d \n", nwrite, errno);

        if (nwrite > 0)
        {
            ptr += nwrite;
            ntowrite -= nwrite;
        }
    }

    // 清除非阻塞状态
    clr_fl(STDOUT_FILENO, O_NONBLOCK);

    exit(0);
}


