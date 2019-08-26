#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

static volatile sig_atomic_t sigflag;

static sigset_t newmask;
static sigset_t oldmask;
static sigset_t zeromask;

/*
    one sig handler for SIGUSR1+SIGUSR2
*/
static void
sig_usr(int signo)
{
    sigflag = 1;
}

void
TELL_WAIT(void)
{
    // 注册信号处理函数，SIGUSR1由父进程发给子进程
    if (signal(SIGUSR1, sig_usr) == SIG_ERR)
        ferror("signal(SIGUSR1) error");
    // 注册信号处理函数，SIGUSR2由子进程发给父进程
    if (signal(SIGUSR2, sig_usr) == SIG_ERR)
        ferror("signal(SIGUSR2) error");

    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);

    // 保存原有的信号屏蔽字，设置新的信号屏蔽字，即将SIGUSR1+SIGUSR2阻塞添加
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        ferror("SIG_BLOCK error");
}

void 
TELL_PARENT(pid_t pid)
{
    // 告知父进程我已结束
    kill(pid, SIGUSR2);
}

void
WAIT_PARENT(void)
{
    // 收到SIGUSR1，即父进程结束，退出，并将信号屏蔽字恢复为newmask
    while (sigflag == 0)
        sigsuspend(&zeromask);  // 等待父进程结束

    sigflag = 0;

    // 恢复原来的信号屏蔽字
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        ferror("SIG_SETMASK error");
}

void 
TELL_CHILD(pid_t pid)
{
    // 告知子进程我已结束
    kill(pid, SIGUSR1);
}

void
WAIT_CHILD(void)
{
    // 收到SIGUSR2，即子进程结束，退出，并将信号屏蔽字恢复为newmask
    while (sigflag == 0)
        sigsuspend(&zeromask);  // 等待子进程结束

    sigflag = 0;

    // 恢复原来的信号屏蔽字
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        ferror("SIG_SETMASK error");
}


#define read_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))    

#define is_read_lockable(fd, offset, whence, len) \
    (lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define is_write_lockable(fd, offset, whence, len) \
    (lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)

/*
    通用的一个接口
*/
int 
lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    return fcntl(fd, cmd, &lock);
}

void
set_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0)
    {
        printf("fcntl F_GETFL error \n");
    }
    
    val |= flags;

    if (fcntl(fd, F_SETFL, val) < 0)
    {
        printf("fcntl F_SETFL error \n");
    }
}

/*
    确定一个系统是否支持强制性锁机制

macOS不支持强制性锁：
./a.out detectlock
read_lock of alread-locked region return errno: 35
read ok no mandatory locking, buf = ab

*/
int main(int argc, char *argv[]) 
{
    int fd;
    pid_t pid;
    char buf[5];
    struct stat statbuf;

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s filename \n", argv[0]);
        exit(1);
    }

    // 创建文件
    if ((fd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, "w")) < 0)
    {
        perror("open error \n");
    }
    // 写入内容
    if (write(fd, "abcdef", 6) != 6)
    {
        perror("write error \n");
    }
    // 获取文件属性
    if (fstat(fd, &statbuf) < 0)
    {
        perror("fstat error \n");
    }
    // 打开组设置ID，关闭组可执行
    if (fchmod(fd, (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
    {
        perror("fchmod error \n");
    }

    // 进程阻塞
    TELL_WAIT();

    // 创建子进程
    if ((pid = fork()) < 0)
        perror("fork error\n");
    // 子进程
    else if (pid == 0)
    {
        // 等待父进程
        WAIT_PARENT();

        // 设置非阻塞
        set_fl(fd, O_NONBLOCK);

        // 加读锁
        if (read_lock(fd, 0, SEEK_SET, 0) != -1)
        {
            printf("child: read_lock successfully \n");
        }
        printf("read_lock of alread-locked region return errno: %d\n", errno);

        // 设置偏移量
        if (lseek(fd, 0, SEEK_SET) == -1)
        {
            perror("lseek error \n");
        }

        // 加读锁之后，能否读取成功？
        if (read(fd, buf, 2) < 0)
        {
            perror("read failed mandatory locking works \n");
        }
        else
        {
            printf("read ok no mandatory locking, buf = %2.2s \n", buf);
        }
        
    }
    // 父进程
    else
    {
        // 对整个文件加写锁
        if (write_lock(fd, 0, SEEK_SET, 0) < 0)
        {
            perror("write_lock error \n");
        }

        // 通知子进程
        TELL_CHILD(pid);

        // 等待子进程
        if (waitpid(pid, NULL, 0) < 0)
        {
            perror("waitpid error \n");
        }
    }
    
    exit(0);
}


