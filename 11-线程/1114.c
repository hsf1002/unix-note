#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

// 定义双向循环链表的工作
struct job
{
    struct job *next;
    struct job *prev;
    pthread_t id;   // 哪个线程处理这个工作

};

// 定义链式队列
struct queue
{
    struct job *head;
    struct job *tail;
    pthread_rwlock_t lock;  // 读写锁
};

/*
    初始化
*/
int 
queue_init(struct queue *qp)
{
    int err;

    qp->head = NULL;
    qp->tail = NULL;

    // 初始化读写锁
    if ((err = pthread_rwlock_init(&qp->lock, NULL)) != 0)
        return(err);

    return(0);
}

/*
    向对头插入一个工作节点
*/
void
job_insert(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->lock);

    jp->next = qp->head;
    jp->prev = NULL;

    // 队列不为空
    if (qp->head != NULL)
        qp->head->prev = jp;
    // 队列为空
    else
        qp->tail = jp;
    
    // 重置对头指针
    qp->head = jp;

    pthread_rwlock_unlock(&qp->lock);
}

/*
    向对尾插入一个工作节点
*/
void
job_append(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->lock);

    jp->next = NULL;
    jp->prev = qp->tail;

    // 队列不为空
    if (qp->tail != NULL)
        qp->tail->next = jp;
    // 队列为空
    else
        qp->head = jp;

    // 重置队尾指针
    qp->tail = jp;

    pthread_rwlock_unlock(&qp->lock);
}

/*
    remove the given job from a queue
*/
void
job_remove(struct queue *qp, struct job *jp)
{
    pthread_rwlock_wrlock(&qp->lock);

    // 删除对头元素
    if (jp == qp->head)
    {
        qp->head = qp->head->next;

        // 队列只有一个元素
        if (qp->tail == jp)
            qp->tail = NULL;
        else
            jp->next->prev = jp->prev;
    }
    // 删除对尾元素
    else if (jp == qp->tail)
    {
        qp->tail = jp->prev;
        jp->prev->next = jp->next;
    }
    // 删除中间元素
    else
    {
        jp->prev->next = jp->next;
        jp->next->prev = jp->prev;
    }
    
    pthread_rwlock_unlock(&qp->lock);
}

/*
    find a job for the given thread ID
*/
struct job *
job_find(struct queue *qp, pthread_t id)
{
    struct job *jp;

    if (pthread_rwlock_rdlock(&qp->lock) != 0)
        return(NULL);
    
    for (jp = qp->head; jp != NULL; jp = jp->next)
        if (pthread_equal(jp->id, id))
            break;
    
    pthread_rwlock_unlock(&qp->lock);

    return(jp);
}

/*
    打印
*/
void queue_print(struct queue *qp)
{
    struct job *jp;

    if (pthread_rwlock_rdlock(&qp->lock) != 0)
        return;
    
    for (jp = qp->head; jp != NULL; jp = jp->next)
        printf("job id = %d \n", jp->id);
    
    pthread_rwlock_unlock(&qp->lock);
}

/*
    添加读写锁的队列

./a.out
job id = 0
job id = 1
job id = 2
job_find, id = 1
job id = 2
*/
int main(void) 
{
    struct queue qp;

    // 初始化
    if (queue_init(&qp) != 0)
        ferror("queue init err \n");
    
    // 对头添加一个
    struct job jp1;
    jp1.id = 0;
    job_insert(&qp, &jp1);

    // 对尾添加一个
    struct job jp2;
    jp2.id = 1;
    job_append(&qp, &jp2);

    // 对尾添加一个
    struct job jp3;
    jp3.id = 2;
    job_append(&qp, &jp3);

    // 打印
    queue_print(&qp);

    // 根据id查找
    struct job *jp4;
    jp4 = job_find(&qp, 1);
    printf("job_find, id = %d \n", jp4->id);

    // 删除jp1,jp2
    job_remove(&qp, &jp1);
    job_remove(&qp, &jp2);

    // 打印
    queue_print(&qp);
}
