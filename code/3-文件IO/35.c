#include <stdio.h>
#include <fcntl.h>

#define BUFFSIZE 4096

/*
    利用read和write复制一个文件
    ./a.out < hi > hello
*/
int main(int argc, char *argv[])
{
    int n;
    char buf[BUFFSIZE];

    while ((n = read(0/*STDIN_FILENO*/, buf, BUFFSIZE)) > 0)
    {
        if (write(1/*STDOUT_FILENO*/, buf, n) != n)
        {
            ferror("write error \n");
        }
    }

    if (n < 0)
    {
        ferror("read error \n");
    }

    exit(0);
}
