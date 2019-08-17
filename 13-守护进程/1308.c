#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

sigset_t mask;

extern int lockfile(int);

/*
    单实例守护进程
*/
int 
already_running(void)
{
    int fd;
    char buf[16];

    // 创建一个文件
    fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
    if (fd < 0)
    {
        syslog(LOG_ERR, "can't open %s: %s \n", LOCKFILE, strerror(errno));
        exit(1);
    }

    // 对此文件进行加锁
    if (lockfile(fd) < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
            return(1);
        }
        syslog(LOG_ERR, "can't lock %s: %s \n", LOCKFILE, strerror(errno));
        exit(1);
    }

    // 对此文件进行截断，内容为空，仅写入pid
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
}

/*
    初始化一个守护进程
*/
void
daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    // 1 设置文件模式屏蔽字
    umask(0);

    // 2 获取文件描述符最大值
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        printf("%s can't get file limit \n", cmd);
    
    // 3 创建子进程，结束父进程
    if ((pid = fork()) < 0)
        printf("%s can't fork \n", cmd);
    else if (pid != 0)
        exit(0);
    
    // 4 无控制终端，成为会话首进程
    setsid();

    // 5 确保将来的打开不会分配控制TTY
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        printf("%s can't ignore SIGHUP \n", cmd);
    if ((pid = fork()) < 0)
        printf("%s can't fork \n", cmd);
    else if (pid != 0)
        exit(0);
    
    // 6 更改当前工作目录为根目录，防止被卸载
    if (chdir("/") < 0)
        printf("%s can't change directory to / \n", cmd);
    
    // 7 关闭所有文件描述符
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i=0; i<rl.rlim_max; ++i)
        close(i);
    
    // 8 丢弃标准输入、标准输出和错误输出
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    // 9 初始化日志文件
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "unexpected file descriptions %d %d %d \n", fd0, fd1, fd2);
        exit(1);
    }
}

/*

*/
void
reread(void)
{
    /* */
    printf("rereading..... \n");
}

/*
*/
void
sigterm(int signo)
{
    printf("got SIGTERM, existing \n");
    exit(0);
}

/*
*/
void
sighangup(int signo)
{
    printf("got SIGHANGUP, re-reading configuration file \n");
    reread();
}

/*
   守护进程可以重读其配置文件的另一种方法
*/
int main(int argc, char *argv[]) 
{
    int err;
    char *cmd;
    struct sigaction sa;

    if ((cmd = strrchr(argv[0], '/')) == NULL)
        cmd = argv[0];
    else
        cmd++;
    
    // 成为守护进程
    daemonize("cmd");

    // 保证单例运行
    if (already_running())
    {
        syslog(LOG_ERR, "daemon alread running");
        exit(1);
    }

    // 处理感兴趣的信号：SIGTERM/SIGHUP
    sa.sa_handler = sigterm; 
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGTERM);
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, NULL) < 0)
        printf("%s can't catch SIGTERM \n", cmd);
    
    sa.sa_handler = sighangup;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGHUP);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        printf("%s can't catch SIGHUP \n", cmd);
    
    /*
        处理其他
    */

    exit(0);
}


