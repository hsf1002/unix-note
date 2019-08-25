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
    suspend的第三种作用：实现父进程、子进程之间的同步
*/

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

/*
    测试一个锁函数
*/
pid_t
lock_test(int fd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    if (fcntl(fd, F_GETLK, &lock) < 0)
        perror("fcntl get lock error \n");
    
    // 失败，已被另一个进程锁住
    if (lock.l_type == F_UNLCK)
        return(0);
    // 成功，返回获取锁的进程ID
    return(lock.l_pid);
}

/*
    对一个字节加锁
*/
static void
lockbyte(const char *name, int fd, off_t offset)
{
    if (writew_lock(fd, offset, SEEK_SET, 1) < 0)
        printf("%s writew_lock error \n", name);
    
    printf("%s got the lock, byte %lld \n", name, (long long)offset);
}

/*
    一个死锁的事例：子进程对第0字节加锁，父进程对第1字节加锁，然后他们每一个都试图对对方已经加锁的字节加锁

./a.out 
create error
: Undefined error: 0
parent got the lock, byte 1 
child got the lock, byte 0 
parent writew_lock error 
parent got the lock, byte 0 
child got the lock, byte 1
*/
int main(int argc, char *argv[]) 
{
    int fd;
    pid_t pid;

    // 创建文件
    if ((fd = creat("templock", "w+")))
        perror("create error\n");
    // 写入内容
    if (write(fd, "ab", 2) != 2)
        perror("write error\n");
    // 进程阻塞
    TELL_WAIT();

    // 创建子进程
    if ((pid = fork()) < 0)
        perror("fork error\n");
    // 子进程
    else if (pid == 0)
    {
        // 子进程对第0个字节加锁-成功
        lockbyte("child", fd, 0);
        // 通知父进程
        TELL_PARENT(getppid());
        // 等待父进程
        WAIT_PARENT();
        // 子进程尝试对第1个字节加锁-失败
        lockbyte("child", fd, 1);
    }
    // 父进程
    else
    {
        // 父进程对第1个字节加锁-成功
        lockbyte("parent", fd, 1);
        // 通知子进程
        TELL_CHILD(pid);
        // 等待子进程
        WAIT_CHILD();
        // 父进程尝试对第0个字节加锁-失败
        lockbyte("parent", fd, 0);
    }
    
    exit(0);
}


