#include <stdio.h>
#include <unistd.h>

#define BUFFSIZE 4096


int main(int argc, char *argv[])
{
    int n;
    char buf[BUFFSIZE];

    // 从标准输入读取
    while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
    {
        // 写到标准输出
        if (write(STDOUT_FILENO, buf, n) != 0)
        {
            printf("write error! \n");
        }
    }

    if (n < 0)
    {
        printf("read error! \n");
    }

    return 0;
}
