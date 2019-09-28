#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <syslog.h>

#if defined(SOLARIS)
#include <netinet/in.h>
#endif

#include <netdb.h>
#include <arpa/inet.h>

#if defined(BSD)
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#define MAXSLEEP 128
#define BUFLEN   128
#define QLEN     10
#define HOST_NAME_MAX 256

#define UPTIME  "/usr/bin/uptime"

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

/**
 * 
 * 连接重试
 */
int
connect_retry(int domain, int type, int protocol, 
            const struct sockaddr *addr, socklen_t alen)
{
    int numsec, fd;

    for (numsec=1; numsec<=MAXSLEEP; numsec <<= 1)
    {
        if ((fd =socket(domain, type, protocol)) < 0)
            return(-1);
        // 面向连接的网络服务如SOCK_STREAM或SOCK_SEQPACKET，必须先使用connect建立连接
        if (connect(fd, addr, alen) == 0)
            return(fd);
        close(fd);

        if (numsec <= MAXSLEEP/2)
            sleep(numsec);
    }

    return(-1);
}

/**
 * 
 * 初始化一个套接字端点提供服务器进程使用
 */
int
initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
    int fd;
    int err = 0;

    // 第一个参数域，地址族，如AF_INET
    // 第二个参数确定套接字的类型，如SOCK_STREAM
    // 第三个参数协议，如IPPROTO_TCP，0表示为给定的域和套接字类型选择默认协议
    if ((fd = socket(addr->sa_family, type, 0)) < 0)
        return(-1);
    // 关联地址和套接字，然后可以通过getsockname发行绑定在套接字上的地址
    if (bind(fd, addr, alen) < 0)
        goto errout;
    if (type == SOCK_STREAM || type == SOCK_SEQPACKET)
        // 服务器调用listen宣告愿意接受连接请求，第二个参数提示系统该进程所要入队的未完成的连接请求数量
        if (listen(fd, qlen) < 0)
            goto errout;
    
    return(fd);

errout:
    err = errno;
    close(fd);
    errno = err;

    return(-1);
}

/**
 * 使用数据报的uptime的服务器版本
 */
void
serve(int sockfd)
{
    int n;
    socklen_t alen;
    FILE *fp;
    char buf[BUFLEN];
    char abuf[BUFLEN];
    struct sockaddr *addr = (struct sockaddr *)abuf;

    //set_cloexec(sockfd);
    fcntl(sockfd, F_SETFD, FD_CLOEXEC);

    for (;;)
    {
        alen = SOCK_MAXADDRLEN;

        // 阻塞等待请求，数据到达时保存对方地址
        if ((n = recvfrom(sockfd, buf, BUFLEN, 0, addr, &alen)) < 0)
        {
            fprintf(stderr, "ruptimed recvfrom error %s", strerror(errno));
            exit(1);
        }

        // 将uptime的输出保存到文件中
        if ((fp = popen(UPTIME, "r")) == NULL)
        {
            sprintf(buf, "error: %s\n", strerror(errno));
            sendto(sockfd, buf, strlen(buf), 0, addr, alen);
        }
        else
        {
            // 将uptime的返回数据发送到该地址
            if (fgets(buf, BUFLEN, fp) != NULL)
                sendto(sockfd, buf, strlen(buf), 0, addr, alen);
            pclose(fp);
        }
    }
}

/**
 * 服务器程序，用来提供uptime命令的输出到16-19的客户端程序
 */
int 
main(int argc, char **argv)
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int sockfd, err, n;
    char *host;

    if (argc != 1)
        perror("usage: ruptimed");
    
    if ((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
        n = HOST_NAME_MAX;
    if ((host = malloc(n)) == NULL)
        perror("malloc error");
    if (gethostname(host, n) < 0)
        perror("gethostname error");
    // 设置为守护进程
    daemonize("ruptimed");

    memset(&hint, 0x00, sizeof(hint));
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_flags = AI_CANONNAME;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    // 如果服务器支持多重网络接口或网络协议，可能返回多个候选地址，轮流尝试，找到允许连接的即可停止
    if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0)
        fprintf(stderr, "getaddrinfo error, %s", gai_strerror(err));
    
    for (aip=ailist; aip!=NULL; aip=aip->ai_next)
    {
        if ((sockfd = initserver(SOCK_DGRAM, aip->ai_addr, aip->ai_addrlen, 0)) < 0)
            err = errno;
        else
        {
            serve(sockfd);
            exit(0);
        }
    }

    exit(1);
}
