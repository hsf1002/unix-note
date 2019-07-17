#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

int quitflag;
sigset_t mask;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitloc = PTHREAD_COND_INITIALIZER;

void
thr_fn(void *arg)
{
    int err;
    int signo;

    for (;;)
    {
        if ((err = sigwait(&mask, &signo)) != 0)
            ferror("sigwait error \n");
        
        switch (signo)
        {
        case SIGINT:
            printf("caught SIGINT, interrupted\n");
            break;
        // 收到SIGQUIT信号，标志位改变
        case SIGQUIT:
            // 使用互斥量修改标志
            pthread_mutex_lock(&lock);
            quitflag = 1;
            pthread_mutex_unlock(&lock);
            // 修改成功后，通知
            pthread_cond_signal(&waitloc);
            //return(0);
            break;
        default:
            printf("unexpected signal %d \n", signo);
            exit(1);
        }
    }
}


/*
    在信号处理程序中设置标志，表明主程序应该退出，阻塞信号可以避免错失标志修改，使用互斥量保护标志

./a.out
^Ccaught SIGINT, interrupted
^Ccaught SIGINT, interrupted
^Ccaught SIGINT, interrupted
^\
*/
int main(void) 
{
    int err;
    sigset_t oldmask;
    pthread_t tid;

    // 初始化信号集，将SIGINT和SIGQUIT添加
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    // 保存旧的信号屏蔽字到oldmask，设置新的，即将SIGINT和SIGQUIT添加到原信号屏蔽字中
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0)
        ferror("SIG_BLOCK error \n");

    // 创建一个线程，并继承了现有的信号屏蔽字
    if ((err = pthread_create(&tid, NULL, thr_fn, 0)) != 0)
        ferror("create thread error \n");
    
    // 使用互斥量检查标志，且原子性的释放互斥量，等待条件发生
    pthread_mutex_lock(&lock);
    // 收到条件发生通知，解除阻塞
    while(quitflag == 0)
        pthread_cond_wait(&waitloc, &lock);
    pthread_mutex_unlock(&lock);

    quitflag = 0;

    // 恢复进程原有的信号屏蔽字
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        ferror("SIG_SETMASK error \n");

    exit(0);
}
