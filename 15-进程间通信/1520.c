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
#include <sys/shm.h>
#include <sys/wait.h>

#define MEMSIZE 1024


/*
    共享内存的简单使用
*/
int 
main(int argc, char *argv[]) 
{
    int shmid;
    pid_t pid;
    char *str;

    // 有亲缘关系的进程key参数可以使用IPC_PRIVATE，且创建共享内存参数不需要IPC_CREATE宏
    if ((shmid = shmget(IPC_PRIVATE, MEMSIZE, 0600)) < 0)
    {
        perror("shmget error");
        exit(1);
    }

    if ((pid = fork()) < 0)
    {
        perror("fork error \n");
        exit(1);
    }
    // 父进程
    else if (pid > 0)
    {
        // 等待子进程结束再运行
        wait(NULL);
        // 关联共享内存
        if ((str = shmat(shmid, NULL, 0)) == (void *)-1)
        {
            perror("shmat error");
            exit(1);
        }
        // 打印共享内存的数据
        puts(str);
        // 分离共享内存
        shmdt(str);
        // 释放共享内存
        shmctl(shmid, IPC_RMID, NULL);

        exit(0);
    }
    // 子进程
    else
    {
        // 关联共享内存
        if ((str = shmat(shmid, NULL, 0)) == (void *)-1)
        {
            perror("shmat error");
            exit(1);
        }
        // 向共享内存写入数据
        strcpy(str, "hello world");
        // 分离共享内存
        shmdt(str);
        // 无需释放

        exit(0);
    }
    
    exit(0);
}

