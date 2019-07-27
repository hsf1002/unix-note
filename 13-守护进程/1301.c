#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <sys/resource.h>
#include <fcntl.h>

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
    创建一个守护进程

ps -efj|grep out
  501  1765     1   0  7:16上午 ??         0:00.00 ./a.out          sky               1764      0    0 S      ?? 
  501  1770   770   0  7:17上午 ttys001    0:00.00 grep out         sky               1769      0    2 S+   s001
*/
int main(void) 
{
    daemonize("main");

    sleep(120);

    exit(0);
}


