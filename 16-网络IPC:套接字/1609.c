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

void print_family(struct addrinfo *aip)
{
    printf(" family ");
    switch (aip->ai_family)
    {
        case AF_INET:
            printf("inet");
            break;
        case AF_INET6:
            printf("inet6");
            break;
        case AF_UNIX:
            printf("unix");
            break;
        case AF_UNSPEC:
            printf("unspecified");
            break;
        
        default:
            printf("unknown");
            break;
    }
}

void print_type(struct addrinfo *aip)
{
    printf(" type ");
    switch (aip->ai_socktype)
    {
        case SOCK_STREAM:
            printf("stream");
            break;
        case SOCK_DGRAM:
            printf("datagram");
            break;
        case SOCK_SEQPACKET:
            printf("seqpacket");
            break;
        case SOCK_RAW:
            printf("raw");
            break;
        
        default:
            printf("unknown type = %d", aip->ai_socktype);
            break;
    }
}

void print_protocol(struct addrinfo *aip)
{
    printf(" protocol ");
    switch (aip->ai_protocol)
    {
        case 0:
            printf("default");
            break;
        case IPPROTO_TCP:
            printf("TCP");
            break;
        case IPPROTO_UDP:
            printf("UDP");
            break;
        case IPPROTO_RAW:
            printf("raw");
            break;
        
        default:
            printf("unknown type = %d", aip->ai_protocol);
            break;
    }
}

void print_flags(struct addrinfo *aip)
{
    printf(" flags ");
    
    if (aip->ai_flags == 0)
    {
        printf(" 0 ");
    }
    else
    {
        if (aip->ai_flags & AI_PASSIVE)
        {
            printf(" passive");
        }

        if (aip->ai_flags & AI_CANONNAME)
        {
            printf(" canon");
        }

        if (aip->ai_flags & AI_NUMERICHOST)
        {
            printf(" numhost");
        }

        if (aip->ai_flags & AI_NUMERICSERV)
        {
            printf(" numserv");
        }

        if (aip->ai_flags & AI_V4MAPPED)
        {
            printf(" v4mapped");
        }

        if (aip->ai_flags & AI_ALL)
        {
            printf(" all");
        }
    }
}

/**
 * 
 * 打印主机和服务信息，说明getaddrinfo用法
 * 
 *  flags  0  family inet type datagram protocol UDP
	 host local address 120.197.89.235 port 2049
    flags  0  family inet type stream protocol TCP
	 host local address 120.197.89.235 port 2049
 */
int main(int argc, char **argv)
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    struct sockaddr_in *sinp;
    const char *addr;
    int err;
    char abuf[INET_ADDRSTRLEN];

    // 如 ./a.out local nfsd
    if (argc != 3)
        fprintf(stderr, "usage: %s nodename service \n", argv[0]);
// (1)AI_PASSIVE当此标志置位时，表示调用者将在bind()函数调用中使用返回的地址结构。当此标志不置位时，表示将在connect()函数调用中使用。
// 当节点名位NULL，且此标志置位，则返回的地址将是通配地址。
// 如果节点名NULL，且此标志不置位，则返回的地址将是回环地址。
// (2)AI_CANNONAME当此标志置位时，在函数所返回的第一个addrinfo结构中的ai_cannoname成员中，应该包含一个以空字符结尾的字符串，字符串的内容是节点名的正规名。
// (3)AI_NUMERICHOST当此标志置位时，此标志表示调用中的节点名必须是一个数字地址字符串
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = 0;
    hint.ai_socktype = 0;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    // 第一个参数可通过hostname获取，第二个参数是服务（进程）名称
    // 将一个主机的服务映射到socket地址列表
    if ((err = getaddrinfo(argv[1], argv[2], &hint, &ailist)) != 0)
        fprintf(stderr, "getaddrinfo error: %s", gai_strerror(err));
    
    for (aip=ailist; aip!=NULL; aip=aip->ai_next)
    {
        print_flags(aip);
        print_family(aip);
        print_type(aip);
        print_protocol(aip);
        // 打印出来的是主机名 local
        printf("\n\t host %s", aip->ai_canonname?aip->ai_canonname:"-");

        if (aip->ai_family == AF_INET)
        {
            sinp = (struct sockaddr_in *)aip->ai_addr;
            // 将数值格式转化为点分十进制的ip地址格式
            addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf, INET_ADDRSTRLEN);
            printf(" address %s", addr?addr:"unknown");
            printf(" port %d", ntohs(sinp->sin_port));
        }
        printf("\n");
    }

    exit(0);
}
