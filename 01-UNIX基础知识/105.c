#include <stdio.h>
#include <unistd.h>


/*
    1. ./a.out > data : 将标准输入从标准输出打印并写到文件
    2. ./a.out < data : 将data中内容读取并显示到终端
    3. ./a.out < data > output: 将文件的内容复制到另一个文件
*/
int main(int argc, char *argv[])
{
    int c;

    // 从标准输入读取
    while ((c = getc(stdin)) != EOF)
    {
        // 写到标准输出
        if (putc(c, stdout) == EOF)
        {
            printf("putc error! \n");
        }
    }

    if (ferror(stdin))
    {
        printf("getc error! \n");
    }

    exit(0);
}
