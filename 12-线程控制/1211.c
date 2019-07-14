#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define MAXSTRINGSZ 4096

static char envbuf[MAXSTRINGSZ];
extern char **environ;

/*
    getenv的一个可能实现：如果两个线程同时调用这个函数，结果不同，因为所有调用getenv的线程返回的字符串
    都保存在同一个静态缓冲区内
*/
char *
getenv(const char *name)
{
    int i;
    int len;

    len = strlen(name);
    for (i=0; environ[i] != NULL; i++)
    {
        if ((strncmp(name, environ[i], len) == 0) && (environ[i][len] == "="))
        {
            strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ-1);
            return(envbuf);
        }
    }

    return(NULL);
}
