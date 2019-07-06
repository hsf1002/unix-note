#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>


/*
    线程回调
*/
void *
thr_fn1(void *arg)
{
    printf("thread1 returning \n");
    return ((void *) 1);
}

/*
    线程回调
*/
void *
thr_fn2(void *arg)
{
    printf("thread2 existing \n");
    pthread_exit((void *) 2);
}

/*
    如何获取已终止线程的退出码

./a.out 
main thread start 
thread1 returning 
thread2 existing 
join after thread1, exit_code = 1 
join after thread2, exit_code = 2 
main thread end
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
    if ((err = pthread_create(&thr1, NULL, thr_fn1, NULL)) != 0)
        printf("can't create thread1, err = %d \n", err);

    // 创建线程2
    if ((err = pthread_create(&thr2, NULL, thr_fn2, NULL)) != 0)
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


