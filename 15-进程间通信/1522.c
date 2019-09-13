#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/msg.h>

typedef union _semun
{
    int val;
    struct semid_ds *buf;
    ushort *array;
}semun;


/**
 * 获取或者创建XSI信号量，POSIX有同名函数sem_init
 * 
 * 1 指定IPC_PRIVATE可以创建
 * 2 定义一个都认可的键
 * 3 通过ftok
 * 
 * 该实例采用第二种方式
 */
static int sem_initial(key_t semkey)
{
    // 获取关键字
    int status;
    int sem_id;

    // 获取信号量，初始值是1，如果是创建，必须指定一个值，如果是引用，则指定为0
    if (-1 == (sem_id = semget(semkey, 1, IPC_CREAT|IPC_EXCL|0644)))
    {
        if (errno == EEXIST)
            // 创建信号量
            sem_id = semget(semkey, 1, 0);
    }
    else
    {
        semun arg;
        // 初始化信号量值
        arg.val = 1;
        // 设置信号量集
        status = semctl(sem_id, 0, SETVAL, arg);
    }
    
    if (sem_id == -1 || status == -1)
    {
        perror("sem init error");
        return(-1);
    }

    return(sem_id);
}

/*
    获取信号量控制的资源
*/
int sem_p(int sem_id)
{
    struct sembuf buf;
    // 0 - nsems-1
    buf.sem_num = 0;
    // 为负值，表明要获取，为0表示调用进程希望等待该信号量值变成0
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO; /* IPC_NOWAIT, SEM_UNDO */

    // semop具有原子性，执行数组中所有操作，或者一个也不做
    if (-1 == semop(sem_id, &buf, 1))
    {
        perror("sem_p error");
        exit(1);
    }

    return 0;
}

/*
    释放信号量控制的资源
*/
int sem_v(int sem_id)
{
    struct sembuf buf;

    buf.sem_num = 0;
    // 为正值，表明要释放，为0表示调用进程希望等待该信号量值变成0
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;

    if (-1 == semop(sem_id, &buf, 1))
    {
        perror("sem_v error");
        exit(1);
    }

    return 0;
}

/*
    删除信号量
*/
int sem_destroy(int sem_id)
{
    return semctl(sem_id, 0, IPC_RMID);
}

/*

*/
void sem_handle(key_t key)
{
    int sem_id;
    int sleep_s = 3;
    pid_t pid = getpid();

    if ((sem_id = sem_initial(key)) < 0)
    {
        exit(1);
    }
    printf("process %d enter \n", pid);
    sem_p(sem_id);
    printf("process %d using resource\n", pid);
    sleep(sleep_s);
    printf("process %d unuse resource\n", pid);
    sem_v(sem_id);
    printf("process %d exist \n", pid);

    exit(0);
}

/*
    XSI信号量的简单使用
*/
int 
main(int argc, char *argv[]) 
{
    key_t key = 0x200;
    pid_t pid;

    // if ((pid = fork()) < 0)
    // {
    //     perror("fork error \n");
    //     exit(1);
    // }
    // // 父进程
    // else if (pid > 0)
    // {
    //     printf("father pid = %d \n", pid);
    //     sem_handle(key);

    //     // 第二个子进程
    //     if ((pid = fork()) == 0)
    //     {
    //         printf("second pid = %d \n", pid);
    //         sem_handle(key);
    //     }

    //     exit(0);
    // }
    // // 第一个子进程
    // else
    // {
    //     printf("first pid = %d \n", pid);
    //     sem_handle(key);
        
    //     exit(0);
    // }

    for (int i=0; i<3; i++)
    {
        if (fork() == 0)
            sem_handle(key);
    }
    
    exit(0);
}

