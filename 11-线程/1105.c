#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

/*
    线程清理程序，仅仅打印
*/
void
clearup(void *arg)
{
    printf("clearup: %s \n", (char *)arg);
}

/*
    线程回调1
*/
void *
thr_fn1(void *arg)
{
    printf("thread1 start \n");
    pthread_cleanup_push(clearup, "thread1 first handler");
    pthread_cleanup_push(clearup, "thread1 second handler");
    printf("thread1 push complete \n");

    // 传入了arg，不为空，正常返回，清理函数不会调用
    if (arg)
        return ((void *)1);
    
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    printf("thread1 pop complete \n");

    return ((void *) 1);
}

/*
    线程回调2
*/
void *
thr_fn2(void *arg)
{
    printf("thread2 start \n");
    pthread_cleanup_push(clearup, "thread2 first handler");
    pthread_cleanup_push(clearup, "thread2 second handler");
    printf("thread2 push complete \n");

    // 传入了arg，不为空，非正常返回，清理函数会调用
    if (arg)
        pthread_exit((void *) 2);
    
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    printf("thread2 pop complete \n");

    pthread_exit((void *) 2);
}

/*
    线程清理程序

./a.out 
main thread start 
thread1 start 
thread1 push complete 
thread2 start 
thread2 push complete 
clearup: thread2 second handler 
clearup: thread2 first handler 
Bus error: 10
*/
int main(void) 
{
    int err;
    pthread_t thr1;
    pthread_t thr2;
    void *tret;

    // 主线程开始
    printf("main thread start \n");  

    // 创建线程1
    if ((err = pthread_create(&thr1, NULL, thr_fn1, (void *)1)) != 0)
        printf("can't create thread1, err = %d \n", err);

    // 创建线程2
    if ((err = pthread_create(&thr2, NULL, thr_fn2, (void *)1)) != 0)
        printf("can't create thread2, err = %d \n", err);

    // 让线程1先执行
    if ((err = pthread_join(thr1, &tret)) != 0)
        printf("can't join after thread1, err = %d \n", err);
    printf("join after thread1, exit_code = %d \n", (long)tret);

    // 让线程2再执行
    if ((err = pthread_join(thr2, &tret)) != 0)
        printf("can't join after thread2, err = %d \n", err);
    printf("join after thread2, exit_code = %d \n", (long)tret);

    // 主线程最后执行
    printf("main thread end \n");  

    exit(0);
}


