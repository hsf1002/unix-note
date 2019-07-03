#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/*
    解除阻塞之后才能调用到，捕获到SIGQUIT信号
*/
static void 
sig_quit(int signo)
{
    printf("caught SIGQUIT \n");
    
    // SIGQUIT的默认动作SIG_DFL是终止进程
    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
        perror("can't reset SIGQUIT");
}

/*
    阻塞的信号，不能发送，一定处于未决状态
    不管产生一次还是五次退出的信号，只发送一次

./a.out 
^\
 SIGQUIT pending 
caught SIGQUIT 
SIGQUIT unblocked 
skydeiMac:10-信号 sky$ ./a.out 
^\^\^\^\^\
 SIGQUIT pending 
caught SIGQUIT 
SIGQUIT unblocked
*/
int
main(void)
{
    sigset_t newmask;   // 设置之后新的信号集
    sigset_t oldmask;   // 设置之前旧的信号集
    sigset_t pendmask;  // 阻塞且未决之信号集

    if (signal(SIGQUIT, sig_quit) == SIG_ERR)
        perror("can't catch SIGQUIT");
    
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);

    // 先将旧的信号集保存到oldmask
    // 再在原信号集上，以阻塞方式添加SIGQUIT的信号
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        perror("SIG_BLOCK error");

    // 信号SIGQUIT在此处会变成未决状态
    sleep(5);

    // 获取进程阻塞且处于未决状态的信号集
    if (sigpending(&pendmask) < 0)
        perror("sigpending error");

    // 信号SIGQUIT是否处于阻塞未决的信号集中
    if (sigismember(&pendmask, SIGQUIT))
        printf("\n SIGQUIT pending \n");

    // 将现有信号集恢复到原先状态，即将SIGQUIT从阻塞未决状态的信号集中删除
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        perror("SIG_SETMASK error");
    
    printf("SIGQUIT unblocked \n");

    // SIGQUIT会在此终止进程，且产生coredump
    sleep(5);
    exit(0);
}
