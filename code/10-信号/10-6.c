#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


static void 
sig_cld(int signo)
{
    pid_t pid;
    int status;

    printf("SIGCLD received \n");

    // 重建handler
    if (signal(SIGCHLD, sig_cld) == SIG_ERR)
        perror("signal error ");

    // 获取子进程状态
    if ((pid = wait(&status)) < 0)
        perror("wait error ");

    printf("pid = %d, status = %d \n", pid, status);
}

/*
 ./a.out 
parent before pause 
child before sleep 
child after sleep 
SIGCLD received 
pid = 71625, status = 0 
parent after pause
*/
int main(int argc, char **argv)
{
    pid_t pid;

    // 注册信号SIGCHLD的处理程序
    if (signal(SIGCHLD, sig_cld) == SIG_ERR)
        perror("signal error \n");

    if ((pid = fork()) < 0)
    {
        perror("fork error \n");
    }
    // 子进程
    else if (pid == 0)
    {
        printf("child before sleep \n");
        sleep(2);
        printf("child after sleep \n");
        _exit(0);
    }

    // 父进程
    printf("parent before pause \n");
    pause();
    printf("parent after pause \n");
    exit(0);
}

