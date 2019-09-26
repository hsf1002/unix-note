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
#define TIMEOUT  20

#define UPTIME  "/usr/bin/uptime"

/*
    
*/
void 
sigalrm(int signo)
{

}

/**
 * 
 */
void
print_uptime(int sockfd, struct addrinfo *aip)
{
    int n;
    char buf[BUFLEN];

    buf[0] = 0;
    // 先发送一个字节的带外数据，服务器收到后会解析出地址
    if (sendto(sockfd, buf, 1, 0, aip->ai_addr, aip->ai_addrlen) < 0)
        perror("sendto 1 byte error");
    alarm(TIMEOUT);
    if ((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0)
    {
        if (errno != EINTR)
            alarm(0);
        perror("recvfrom error");
    }
    alarm(0);
    write(STDOUT_FILENO, buf, n);
}

/**
 * 采用数据报服务的客户端程序
 */
int 
main(int argc, char **argv)
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int sockfd, err, n;
    char *host;
    struct sigaction sa;

    if (argc != 2)
        perror("usage: ruptime hostname ");
    
    sa.sa_handler = sigalrm;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGALRM, &sa, NULL) < 0)
        perror("sigaction error");

    memset(&hint, 0x00, sizeof(hint));
    hint.ai_socktype = SOCK_DGRAM;
    //hint.ai_flags = AI_CANONNAME;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    // 如果服务器支持多重网络接口或网络协议，可能返回多个候选地址，轮流尝试，找到允许连接的即可停止
    if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
        fprintf(stderr, "getaddrinfo error, %s", gai_strerror(err));
    
    for (aip=ailist; aip!=NULL; aip=aip->ai_next)
    {
        if ((sockfd = socket(aip->ai_family, SOCK_DGRAM, 0)) < 0)
            err = errno;
        else
        {
            print_uptime(sockfd, aip);
            exit(0);
        }
    }

    fprintf(stderr, "cannot connect to %s: %s", argv[1], strerror(err));
    exit(1);
}
