#include <stdio.h>
#include <unistd.h>

#define MAXLINE 100

/*
$ ./a.out 
% date
2019年 6月 8日 星期六 10时37分30秒 CST
who
sky      console  Apr 25 22:18 
sky      ttys000  Apr 28 22:31 
sky      ttys001  May  2 21:33 
sky      ttys002  May 14 06:28 
pwd
/Users/sky/work/practice/unix-note/code/1-UNIX基础知识
ls
13.c	14.c	15.c	17.c	a.out
^C
    
    在终端循环执行某个程序
*/
int main(int argc, char *argv[])
{
    char buf[MAXLINE];
    pid_t pid;
    int status;
    // 区别于$
    printf("%% ");

    while (fgets(buf, MAXLINE, stdin) != NULL)
    {
        if (buf[strlen(buf) - 1] == '\n')
        {
            buf[strlen(buf) - 1] = 0;   /* replace newline with null */
        }

        if ((pid = fork()) < 0)
        {
            ferror("fork error");
        }
        /* child */
        else if (pid == 0)
        {
            execlp(buf, buf, (char *)0);
            printf("couldnot execute: %s \n", buf);
            exit(127);
        }
        
        /* parent */
        if ((pid = waitpid(pid, &status, 0)) < 0)
        {
            printf("waitpid error! \n");
        }
    }

    exit(0);
}
