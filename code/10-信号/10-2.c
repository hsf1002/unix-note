#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// 两个信号的处理函数
static void sig_usr(int);

/*
./a.out &
[2] 70669
skydeiMac:10-信号 sky$ kill -USR1 70669
received SIGUSR1 
skydeiMac:10-信号 sky$ kill -USR2 70669
received SIGUSR2 
skydeiMac:10-信号 sky$ kill 70669
[2]-  Terminated: 15          ./a.out 
*/
int main(int argc, char **argv)
{
    if (signal(SIGUSR1, sig_usr) == SIG_ERR)
        perror("can't catch SIGUSR1");

    if (signal(SIGUSR2, sig_usr) == SIG_ERR)
        perror("can't catch SIGUSR2");

    for (;;)
        pause();
}

static void sig_usr(int signo)
{
    if (signo == SIGUSR1)
        printf("received SIGUSR1 \n");
    else if (signo == SIGUSR2)
        printf("received SIGUSR2 \n");
    else
        printf("received signal: %d \n", signo);
}
