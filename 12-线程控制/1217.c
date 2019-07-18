#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <signal.h>


pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

/*
    在fork创建子进程前调用，任务是获取父进程定义的所有锁
*/
void
prepare(void)
{
    int err;

    printf("prepare to lock...\n");

    if ((err = pthread_mutex_lock(&lock1)) != 0)
        ferror("can't lock lock1 in prepare \n");

    if ((err = pthread_mutex_lock(&lock2)) != 0)
        ferror("can't lock lock2 in prepare \n");
}

/*
    在fork创建子进程后，返回之前在父进程的上下文中调用，任务是对prepare获取的所有锁进行解锁
*/
void
parent(void)
{
    int err;

    printf("parent to unlock...\n");

    if ((err = pthread_mutex_unlock(&lock1)) != 0)
        ferror("can't unlock lock1 in parent \n");

    if ((err = pthread_mutex_unlock(&lock2)) != 0)
        ferror("can't unlock lock2 in parent \n");
}

/*
    在fork返回之前，在子进程上下文中调用，任务是释放prepare获取的所有锁
*/
void
child(void)
{
    int err;

    printf("child to unlock...\n");

    if ((err = pthread_mutex_unlock(&lock1)) != 0)
        ferror("can't unlock lock1 in child \n");

    if ((err = pthread_mutex_unlock(&lock2)) != 0)
        ferror("can't unlock lock2 in child \n");
}

/*
    sleep(2)结束之后，结束
*/
void *
thr_fn(void *arg)
{
    printf("thread started ...\n");
    pause();
    return(0);
}


/*
    如何使用pthread_atfork和fork处理程序

 ./a.out
thread started ...
parent about to fork...
prepare to lock...
parent to unlock...
parent returned from fork
child to unlock...
child returned from fork
*/
int main(void) 
{
    int err;
    pid_t pid;
    pthread_t tid;

    // 通过pthread_atfork安装清理锁的函数
    if ((err = pthread_atfork(prepare, parent, child)) != 0)
        ferror("can't install fork handlers \n");

    // 创建一个线程
    if ((err = pthread_create(&tid, NULL, thr_fn, 0)) != 0)
        ferror("create thread error \n");
    
    sleep(2);
    printf("parent about to fork...\n");

    if ((pid = fork()) < 0)
        ferror("fork failed \n");
    else if (pid == 0)
        printf("child returned from fork \n");
    else
        printf("parent returned from fork \n");

    exit(0);
}


