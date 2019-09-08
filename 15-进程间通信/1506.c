#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

static int pfd1[2], pfd2[2];

/*
    使用管道实现五个函数
*/

/*
    等待
*/
void 
TELL_WAIT(void)
{
    if (pipe(pfd1) < 0 || pipe(pfd2) < 0)
        perror("TELL_WAIT pipe error");
}

/*
    通知父进程
*/
void 
TELL_PARENT(pid_t pid)
{
    if (write(pfd2[1], "c", 1) != 1)
        perror("TELL_PARENT write error");
}

/*
    等待父进程
*/
void 
WAIT_PARENT(void)
{
    char c;

    if (read(pfd1[1], &c, 1) != 1)
        perror("WAIT_PARENT read error");

    if (c != 'p')
        perror("WAIT_PARENT, incorrect data");
}

/*
    通知子进程
*/
void
TELL_CHILD(pid_t pid)
{
    if (write(pfd1[1], "p", 1) != 1)
        perror("TELL_CHILD write error");
}

/*
    等待子进程
*/
void 
WAIT_CHILD(void)
{
    char c;

    if (read(pfd2[0], &c, 1) != 1)
        perror("WAIT_CHILD read error");
    
    if (c != 'c')
        perror("WAIT_CHILD incorrect data");
}
