#include <stdio.h>

#define MAXLINE 100


/*
    tmpnam和tmpfile

skydeiMac:5-标准IO库 sky$ ./a.out
/var/tmp/tmp.0.OSo7zq
/var/tmp/tmp.1.iyio65
one line of output
*/
int main(int argc, char *argv[])
{
    FILE *fp;
    char name[L_tmpnam], line[MAXLINE];

    // 第一次产生的名字，存放在静态区
    printf("%s\n", tmpnam(NULL));

    // 第二次产生的名字
    tmpnam(name);
    printf("%s\n", name);

    // 创建二进制临时文件（wb+）
    if ((fp = tmpfile()) == NULL)
    {
        ferror("tmpfile error");
    }

    // 向临时文件写内容
    fputs("one line of output \n", fp);
    // 重置文件指针到文件头
    rewind(fp);

    // 读取临时文件内容
    if (fgets(line, sizeof(line), fp) == NULL)
    {
        ferror("fgets error");
    }

    // 将临时文件内容输出到标准输出
    fputs(line, stdout);
   
    return(0);
}
