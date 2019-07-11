#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/*
    fork两次的作用：如果一个进程fork子进程，但不要等待子进程终止，也不希望子进程处于僵死状态直到父进程终止

./a.out 
the first child, pid = 62331 
the first child, ppid = 62330 
the parent, pid = 62330 
...
// 第二个子进程等待了1秒，此时其父进程-即第一个子进程已经死亡，所以它的父进程变成了init进程
the second child, parent pid = 1
*/
int main(int argc, char **argv)
{
    pid_t pid;

    // 第一次fork
    if ((pid = fork()) < 0)
    {
        ferror("first fork error");
    }
    // 第一个子子进程
    else if(pid == 0)   
    {
        // 第二次fork
        if ((pid = fork()) < 0)
        {
            ferror("second fork error");
        }
        else if(pid > 0)
        {
            printf("the first child, pid = %d \n", (long)getpid());
            printf("the first child, ppid = %d \n", (long)getppid());
            exit(0);    
        }
        // 第二个子进程，它的父进程是第一个子进程
        sleep(1);
        printf("the second child, parent pid = %d \n", (long)getppid());
        exit(0);
    }

    // 等待第一个子进程
    if (waitpid(pid, NULL, 0) != pid)
    {
        ferror("waitpid error \n");
    }
    
    // 父进程，继续执行
    printf("the parent, pid = %d \n", (long)getpid());

    exit(0);
}
