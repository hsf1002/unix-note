#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/*
    解释器文件
*/
int main(int argc, char **argv)
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        ferror("first fork error");
    }
    else if(pid == 0)   
    {
        if (execl("/Users/sky/work/practice/unix-note/code/8-进程控制/testinterp", "testinterp", "myarg1", "My Arg2", (char *)0) < 0)
        {
            ferror("execle error");
        }
    }

    if (waitpid(pid, NULL, 0) < 0)
    {
        ferror("wait error");
    }

    exit(0);
}
