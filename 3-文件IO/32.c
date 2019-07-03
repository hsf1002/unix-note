#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

static char buf1[] = "abcdefghij";
static char buf2[] = "ABCDEFGHIJ";

/*
    创建具有空洞的文件

-rwxr-xr-x  1 sky  staff  16394  6  8 15:11 file.hole*
skydeiMac:3-文件IO sky$ od -c file.hole
0000000    a   b   c   d   e   f   g   h   i   j  \0  \0  \0  \0  \0  \0
0000020   \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0  \0
*
0040000    A   B   C   D   E   F   G   H   I   J
0040012

    od 用于查看文件的实际内容，-c表示以字符方式打印文件内容，每行开始的7位数以8进制形式表示的字节偏移量
*/
int main(int argc, char *argv[])
{
    int fd;

    // 新建空文件
    if ((fd = creat("file.hole", 0755)) < 0)
    {
        ferror("create error \n");
    }

    // 写入10个字节
    if (write(fd, buf1, 10) != 10)
    {
        ferror("buf1 write error \n");
    }

    // 文件指针移动到16384的位置
    if (lseek(fd, 16384, SEEK_SET) == -1)
    {
        ferror("seek error \n");
    }

    // 继续写入10个字节
    if (write(fd, buf2, 10) != 10)
    {
        ferror("buf2 write error \n");
    }

    exit(0);
}
