#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>

static volatile sig_atomic_t quitflag;

/*
    one sig handler for SIGINT+SIGQUIT
*/
static void
sig_init(int signo)
{
    if (signo == SIGINT)
        printf("\n interrupted \n");
    else if (signo == SIGQUIT)
    {
        printf("\n quit \n");
        quitflag = 1;
    }
}


/*
    suspend的第二种作用：等待一个信号处理程序设置一个全局变量

./a.out 
^C
 interrupted 
^C
 interrupted 
^C
 interrupted 
^C
 interrupted 
^C
 interrupted 
^\
 quit 
*/
int 
main(void)
{
    sigset_t newmask;
    sigset_t oldmask;
    sigset_t zeromask;

    // 注册SIGINT的信号处理函数
    if (signal(SIGINT, sig_init) == SIG_ERR)
        ferror("signal(SIGINT) error");

    // 注册SIGQUIT的信号处理函数
    if (signal(SIGQUIT, sig_init) == SIG_ERR)
        ferror("signal(SIGQUIT) error");

    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);

    // 旧的信号屏蔽字保存在oldmask，且设置新的屏蔽字，即在原有屏蔽字基础上，将信号SIGQUIT阻塞添加
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        perror("SIG_BLOCK error");

    // 暂停，阻塞在此处，收到SIGQUIT信号，设置quitflag为true后，信号屏蔽字恢复为newmask，从循环中退出
    while (quitflag == 0)
        sigsuspend(&zeromask);

    // 重置
    quitflag = 0;

    // 恢复旧的信号屏蔽字，信号SIGQUIT不再阻塞
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        perror("SIG_SETMASK error");

    exit(0);
}

