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
#include <semaphore.h>

#define _POSIX_NAME_MAX 128

typedef struct 
{
    sem_t *semp;
    char name[_POSIX_NAME_MAX];
}slock;

/*
    初始化，创建信号量
*/
slock *
s_aloc()
{
    slock *sp;
    static int cnt;

    if ((sp = malloc(sizeof(slock))) == NULL)
        return(NULL);
    do
    {
        // 信号量名称规则：
        // 1. 第一个字符应该为斜杠/
        // 2. 不应包含其他斜杠以此避免实现定义的行为
        // 3. 最大长度是实现定义，不应超过_POSIX_NAME_MAX
        snprintf(sp->name, sizeof(sp->name), "/%ld.%d", (long)getpid(), cnt++);
        // 创建或引用一个信号量，创建才需要指定第二三个参数，最后一个参数为信号量初始值
        sp->semp = sem_open(sp->name, O_CREAT|O_EXCL, S_IRWXU, 1);
        // 可以创建未命名信号量
        // 有名信号量一般是用在进程间同步，无名信号量一般用在线程间同步
        // sem_init(sp->semp, IPC_PRIVATE, 1);
    } while ((sp->semp == SEM_FAILED) && (errno == EEXIST));
    
    if (sp->semp == SEM_FAILED)
    {
        free(sp);
        return(NULL);
    }
    // 销毁一个命名信号量
    sem_unlink(sp->name);
    return(sp);
}

/**
 * 销毁信号量
 */
void 
s_free(slock *sp)
{
    // 释放命名信号量相关的资源
    sem_close(sp->semp);
    // 释放未命名信号量相关的资源
    //sem_destroy(sp->semp);
    free(sp);
}

/*
    等待获取信号量
 */
int
s_lock(slock *sp)
{
    // 实现减一操作，阻塞
    return(sem_wait(sp->semp));
}

/*
    尝试等待获取信号量
 */
int 
s_trylock(slock *sp)
{
    // 实现减一操作，不阻塞，直接返回
    return(sem_trywait(sp->semp));
}

/*
    释放信号量
 */
int
s_unlock(slock *sp)
{
    // 实现加一操作
    return(sem_post(sp->semp));
}

/*
    检索信号量的值，macOS10.6.8 不支持
 */
int 
s_getval(slock *sp, int *value)
{
    return(sem_getvalue(sp->semp, value));
}

/**
 * 
 */
void sem_handle()
{
  slock *sp;
  int sleep_s = 3;
  pid_t pid = getpid();
  
  if (NULL == (sp = s_aloc()))
    exit(1);

  printf("process %d enter \n", pid);
  s_lock(sp);                                      
  printf("process %d using \n", pid);
  sleep(sleep_s);
  printf("process %d unuse \n", pid);
  s_unlock(sp);                                
  printf("process %d exist \n", pid);

  exit(0);
}

/*
    POSIX信号量实现互斥原语
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
            sem_handle();
    }
    
    exit(0);
}

