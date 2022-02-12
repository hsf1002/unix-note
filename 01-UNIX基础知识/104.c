#include <stdio.h>
#include <unistd.h>

#define BUFFSIZE 4096

/*
    1. ./a.out > data : 将标准输入从标准输出打印并写到文件
    2. ./a.out < data : 将data中内容读取并显示到终端
    3. ./a.out < data > output: 将文件的内容复制到另一个文件
*/
int main(int argc, char *argv[])
{
    int n;
    char buf[BUFFSIZE];

    // 从标准输入读取
    while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
    {
        // 写到标准输出
        if (write(STDOUT_FILENO, buf, n) != n)
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
