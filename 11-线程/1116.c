#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <limits.h>

// 工作线程的个数
#define NTHR    8
// 需要排序的数据的个数
#define NUMNUM  8000000L
// 每个工作线程需要处理的数据的个数
#define TNUM  (NUMNUM/NTHR)

// 需要排序的数据
long nums[NUMNUM];
//
long snums[NUMNUM];

pthread_barrier_t b;

#ifdef SOLARIS
#define heapsort qsort
#else
extern int heapsort(void *, size_t, int (*)(const void *, const void *));
#endif


/*
    比较两个数据的大小
*/
int 
complong(const void *arg1, const void *arg2)
{
    long l1 = *(long *)arg1;
    long l2 = *(long *)arg2;

    if (l1 == l2)
        return 0;
    else if (l1 < l2)
        return -1;
    else
        return 1;
}

/*
    每个工作线程需要处理的数据
*/
void *
thr_fn(void *arg)
{
    long idx = (long)arg;

    heapsort(&nums[idx], TNUM, sizeof(long),  complong);
    // 每个工作线程都阻塞在此，等待最后的召唤
    thread_barrier_wait(&b);
    // 所有工作线程统一唤醒结束
    printf("thread %d finished \n", idx / TNUM);
    return ((void *)0);
}

/*
    对已经排序的各个数组，进行合并
*/
void
merge()
{
    long idx[NTHR];
    long i;
    long minidx;
    long sidx;
    long num;

    for (i=0; i<NTHR; i++)
        idx[i] = i * TNUM;
    
    for (sidx=0; sidx<NUMNUM; sidx++)
    {
        num = LONG_MAX;
        for (i=0; i<NTHR; i++)
        {
            if ((idx[i] < (i+1)*TNUM) && (nums[idx[i]] < num))
            {
                num = nums[idx[i]];
                minidx = i;
            }
        }
        snums[sidx] = nums[idx[minidx]];
        idx[minidx]++;
    }
}

/*
    使用栅栏（屏障）给800万个数据进行堆排序
*/
int main(void) 
{
    unsigned long i;
    struct timeval start;
    struct timeval end;
    long long startusec;
    long long endusec;
    double elapse;
    int err;
    pthread_t tid;

    // 主线程开始
    printf("main thread start \n"); 

    // 生成随机数
    srandom(1);
    for (i=0; i<NUMNUM; i++)
        nums[i] = random();

    gettimeofday(&start, NULL);
    // 栅栏初始化，设置9个线程，其中8个工作线程+主线程
    pthread_barrier_init(&b, NULL, NTHR+1);
        
    // 创建8个工作线程
    for (i=0; i<NTHR; i++)
    {
        // 传递的参数是数据集的起点，如0，100万，200万，300万....
        if ((err = pthread_create(&tid, NULL, thr_fn, (void *)(i * TNUM))) != 0)
            printf("can't create thread%d, err = %d \n", i, err);
    }

    // 主线程也等待
    pthread_barrier_wait(&b);
    // 8个工作线程排序完毕，开始合并
    merge();
    gettimeofday(&end, NULL);

    // 打印排序后的数据
    startusec = start.tv_sec * 1000000 + start.tv_usec;
    endusec = end.tv_sec * 1000000 + end.tv_usec;
    elapse = (double)(endusec - startusec) / 1000000.0;
    printf("sort 800,0000 data took %.4f seconds \n", elapse);

    for (i=0; i<NUMNUM; i++)
    {
        printf("%ld \t", snums[i]);

        if (i % 10 == 0)
            printf("\n");
    }

    printf("main thread end \n");  

    exit(0);
}


