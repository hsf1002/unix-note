#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

struct msg
{
    struct msg *next;
    /* more stuff here */
};

// 工作队列
struct msg *workq;
// 初始化条件变量
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
// 初始化互斥锁
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

/*
    入队操作
*/
void enqueue_msg(struct msg *mp)
{
    // 获取锁
    pthread_mutex_lock(&qlock);
    mp->next = workq;
    workq = mp;
    // 释放锁
    pthread_mutex_unlock(&qlock);
    // 唤醒等待的线程
    pthread_cond_signal(&qready);
}

/*
    出队操作
*/
void process_msg(void)
{
    struct msg *mp;

    for (;;)
    {
        // 获取锁
        pthread_mutex_lock(&qlock);
        // 如果队列为空，一直等待
        while(workq == NULL)
            pthread_cond_wait(&qready, &qlock);
        // 唤醒之后
        mp = workq;
        workq = mp->next;
        // 释放锁
        pthread_mutex_unlock(&qlock);
        /* now process the message mp */
    }
}
