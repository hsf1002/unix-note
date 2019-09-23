#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>

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
 * 从服务器读取数据，并打印
 */
void
print_uptime(int sockfd)
{
    int n;
    char buf[BUFLEN];

    while((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
        write(STDOUT_FILENO, buf, n);
    
    if (n < 0)
        perror("recv error");
}

/**
 * 连接服务器，读取服务器发送过来的字符串，将其打印到标准输出
      客户端，需要服务器配合
 */
int 
main(int argc, char **argv)
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int sockfd, err;

    if (argc != 3)
        perror("usage: hostname ruptimed");
    memset(&hint, 0x00, sizeof(hint));
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    // 如果服务器支持多重网络接口或网络协议，可能返回多个候选地址，轮流尝试，找到允许连接的即可停止
    if ((err = getaddrinfo(argv[1], argv[2], &hint, &ailist)) != 0)
        fprintf(stderr, "getaddrinfo error, %s", gai_strerror(err));
    
    for (aip=ailist; aip!=NULL; aip=aip->ai_next)
    {
        if ((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0, aip->ai_addr, aip->ai_addrlen)) < 0)
            err = errno;
        else
        {
            print_uptime(sockfd);
            exit(0);
        }
    }

    fprintf(stderr, "cannot connect to %s", argv[1]);
}
