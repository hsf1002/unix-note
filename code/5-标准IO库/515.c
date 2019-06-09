#include <stdio.h>
#include <sys/stat.h>
#include <sys/errno.h>

#define BSZ 48


/*

./a.out
initial buffer contents:
before fflush: hello world
after fflush: hello world
size: 11
after seek: bbbbbbbbbbbhello world
size: 22
after fclose: hello world
size: 11

*/
int main(int argc, char *argv[])
{
    FILE *fp;
    char buf[BSZ];

    // 用字符a填充缓冲区
    memset(buf, 'a', BSZ - 2);
    buf[BSZ - 2] = '\0';
    buf[BSZ - 1] = 'X';

    // fmemopen在缓冲区开始处放置null字节
    if ((fp = fmemopen(buf, BSZ, "w+")) == NULL)
    {
        ferror("fmemopen failed");
    }

    printf("initial buffer contents: %s \n", buf);

    // 冲洗后缓冲区内容才有变化
    fprintf(fp, "hello world");
    printf("before fflush: %s \n", buf);
    fflush(fp);
    printf("after fflush: %s \n", buf);
    printf("size: %ld \n", (long)strlen(buf));

    // 用字符b填充缓冲区
    memset(buf, 'b', BSZ - 2);
    buf[BSZ - 2] = '\0';
    buf[BSZ - 1] = 'X';

    // fseek引起缓冲区冲洗，再次追加null字节
    fprintf(fp, "hello world");
    fseek(fp, 0, SEEK_SET);
    printf("after seek: %s \n", buf);
    printf("size: %ld \n", (long)strlen(buf));

    // 用字符c填充缓冲区
    memset(buf, 'c', BSZ - 2);
    buf[BSZ - 2] = '\0';
    buf[BSZ - 1] = 'X';

    // 没有追加null字节
    fprintf(fp, "hello world");
    fclose(fp);
    printf("after fclose: %s \n", buf);
    printf("size: %ld \n", (long)strlen(buf));

    return(0);
}

