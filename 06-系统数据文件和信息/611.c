#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*
    time、localtime、strftime    

buffer len 16 is too small 
time and date: 11:34:11 AM, Thu Sep 17, 2020

*/
int main(int argc, char *argv[])
{
    time_t t;
    struct tm *tmp;
    char buf1[16];
    char buf2[64];

    // 获取日历时间
    time(&t);
    // 将日历时间转为本地时间
    tmp = localtime(&t);

    // 将格式化时间的字符串保存到buf
    if (strftime(buf1, 16, "time and date: %r, %a %b %d, %Y", tmp) == 0)
    {
        printf("buffer len 16 is too small \n");
    }
    else
    {
        printf("%s\n", buf1);
    }
    
    if (strftime(buf2, 64, "time and date: %r, %a %b %d, %Y", tmp) == 0)
    {
        printf("buffer len 64 is too small \n");
    }
    else
    {
        printf("%s\n", buf2);
    }

    return(0);
}

