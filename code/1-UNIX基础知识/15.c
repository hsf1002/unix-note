#include <stdio.h>
#include <unistd.h>


/*
    	1. ./.out: 出将标准输入的内容一个字符一个字符打印到标准输出
	2. ./.out < input > output: 将文件的内容复制到另一个文件
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
