#include <stdio.h>
#include <unistd.h>


/*
    测试标准输入能发设置偏移量

./a.out < /etc/passwd 
seek ok 
skydeiMac:3-文件IO sky$ cat /etc/passwd | ./a.out 
cannot seek     
*/
int main(int argc, char *argv[])
{
    if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1)
    {
        printf("cannot seek \n");
    }
    else
    {
        printf("seek ok \n");
    }

    exit(0);
}
