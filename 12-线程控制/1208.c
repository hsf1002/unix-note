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


struct to_info
{
    void (*to_fn)(void *);  /* function */
    void *to_arg;           /* argument */
    struct timespec to_wait;/* time to wait */
};

#define SECTONSEC  100000000    /* seconds to nanoseconds */

#if !defined(CLOCK_REALTIME) defined(BSD)
#define clock_nanosleep(ID, FL, REQ, REM) nanosleep((REQ), (REM))
#endif

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0

#define USECTONSEC 1000     /* microseconds to nanoseconds */

void
clcok_gettime(int id, struct timespec *tsp)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    tsp->tv_sec = tv.tv_sec;
    tsp->tv_sec = tv.tv_usec * USECTONSEC;
}
#endif

void
timeout_helper(void *arg)
{
    struct to_info *tip;

    tip = (struct to_info*)arg;
    clcok_nanosleep(CLOCK_REALTIME, 0, &tip->to_wait, NULL);
    (*tip->to_fn)(tip->to_arg);
    free(arg);
    return(0);
}

void
timeout(const struct timespec *when, void (*func)(void *), void *arg)
{
    struct timespec now;
    struct to_info *tip;
    int err;

    clock_gettime(CLOCK_REALTIME, &now);

    if (when->tv_sec > now.tv_sec || \
        (when->tv_sec == now.tv_sec && when->tv_sec > now.tv_nsec))
    {
        tip = malloc(sizeof(struct to_info));

        if (NULL != tip)
        {
            tip->to_fn = func;
            tip->to_arg = arg;
            tip->to_wait.tv_sec = when->tv_sec - now.tv_sec;

            if (when->tv_nsec >= now.tv_nsec)
            {
                tip->to_wait.tv_nsec = when->tv_nsec - now.tv_nsec;
            }
            else
            {
                tip->to_wait.tv_sec--;
                tip->to_wait.tv_nsec = SECTONSEC - now.tv_nsec + when->tv_nsec;
            }

            err = makethread(timeout_helper, (void *)tip);

            if (0 == err)
                return;
            else
                free(tip);
        } 
    }
}

pthread_mutexattr_t attr;
pthread_mutex_t mutex;

void 
retry(void *arg)
{
    pthread_mutex_lock(&mutex);

    /* perform retry steps */

    pthread_mutex_unlock(&mutex);
}

/*
    说明了有必要使用递归互斥量的另一种情况：为每个挂起的超时函数创建一个线程，线程在时间未到时一直等待
        时间到了就调用请求的函数
    
*/
int main(void) 
{
    int err;
    int arg;
    int condition;
    struct timespec when;

    // 初始化互斥量属性
    if ((err = pthread_mutexattr_init(&attr)) != 0)
        perror("pthread_mutexattr_init error \n");
    // 设置互斥量属性的类型，为可递归加锁
    if ((err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)) != 0)
        perror("pthread_mutexattr_settype error \n");
    // 初始化互斥量
    if ((err = pthread_mutex_init(&mutex, &attr)) != 0)
        perror("pthread_mutex_init error \n");
    
    /* 继续其他 */

    // 互斥量加锁
    pthread_mutex_lock(&mutex);

    // 锁保护的情况下检查条件变量，使得检查操作和超时调用成为一个原子操作
    if (condition)
    {
        // 当重试的时候，计算绝对时间
        clock_gettime(CLOCK_REALTIME, &when);
        when.tv_sec += 10;
        timeout(&when, retry, (void *)((unsigned long)arg));
    }

    // 互斥量解锁
    pthread_mutex_unlock(&mutex);
   
    /* 继续其他 */

    exit(0);
}


