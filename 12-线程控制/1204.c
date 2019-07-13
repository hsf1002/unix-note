#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>


/*
    以分离状态创建线程
*/
int
makethread(void *(*fn)(void *), void *arg)
{
    int err;
    pthread_t tid;
    pthread_attr_t attr;

    // 初始化线程属性
    if (0 != (err = pthread_attr_init(&attr)))
        return(err);
    
    // 设置线程属性为分离状态
    if (0 == (err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)))
        err = pthread_create(&tid, &attr, fn, arg);

    // 销毁线程属性
    pthread_attr_destroy(&attr);
    return(err);
}

