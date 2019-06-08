#include <stdio.h>
#include <fcntl.h>


/*
    查看文件标志，第一个参数是文件描述符

./a.out 0 < /dev/tty
read only
skydeiMac:3-文件IO sky$ ./a.out 1 > temp.foo
skydeiMac:3-文件IO sky$ cat temp.foo 
write only
skydeiMac:3-文件IO sky$ ./a.out 2 2>>temp.foo 
write only, append
skydeiMac:3-文件IO sky$ ./a.out 5 5<>temp.foo 
read write

1 > temp.foo表示在文件描述符1上打开文件以写
2>>temp.foo表示在文件描述符2上打开文件以追加
5<>temp.foo表示在文件描述符5上打开文件以读写
*/
int main(int argc, char *argv[])
{
    int val;

    if (argc != 2)
    {
        printf("usage: ./out < description \n");
        return -1;
    }

    if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
    {
        printf("fcntl error for fd %d \n", atoi(argv[1]));
        return -1;
    }

    switch (val & O_ACCMODE)
    {
        case O_RDONLY:
            printf("read only");
            break;
        case O_WRONLY:
            printf("write only");
            break;
        case O_RDWR:
            printf("read write");
            break;
        default:
            printf("unknown mode");
            break;
    }

    if (val & O_APPEND)
    {
        printf(", append");
    }
    
    if (val & O_NONBLOCK)
    {
        printf(", nonblocking");
    }

    if (val & O_SYNC)
    {
        printf(", sync");
    }

#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC) && (O_FSYNC != O_SYNC)
    if (val & O_FSYNC)
    {
        printf(, synchronous write);
    }
#endif

    putchar('\n');

    exit(0);
}
