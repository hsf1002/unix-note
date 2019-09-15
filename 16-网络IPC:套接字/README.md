### 第十六章  网络IPC：套接字

##### 套接字描述

套接字是通信端点的抽象，如使用文件描述符访问文件，用套接字描述符访问套接字，套接字描述符在UNIX中被当做一种文件描述符

创建套接字：

```
#include <sys/socket.h>

int socket(int domain, int type, int protocal);
// 若成功，返回文件（套接字）描述符，若出错，返回-1
```

domain表示地址族

```
AF_INET: IPv4
AF_INET6: IPv6
AF_UNIX: UNIX域
AF_UPSPEC: 未指定
```

type表示套接字的类型

```
SOCK_DGRAM: 固定长度的、无连接的、不可靠的报文传递
SOCK_RAW: IP协议的数据报接口
SOCK_SEQPACKET: 固定长度的、有序的、可靠的、面向连接的报文传递
SOCK_STREAM: 有序的、可靠的、双向的、面向连接的字节流

AF_INET中，SOCK_STREAM的默认协议是TCP，SOCK_DGRAM的默认协议是UDP
```

protocol表示协议的类型

```
IPPROTO_IP: IPv4
IPPROTO_IPV6: IPv6
IPPROTO_ICMP: 因特网控制报文协议（Internet Control Message Protocal）
IPPROTO_RAW: 原始IP数据包协议
IPPROTO_TCP: 传输控制协议（Transmission Control Protocol）
IPPROTO_UDP: 用户数据报协议（User Datagram Protocol）
```

对于数据报SOCK_DGRAM，两个对等进程之间通信时不需要逻辑连接，只需要向对等进程的套接字发送一个报文，因此提供了一个无连接的服务；字节流SOCK_STREAM在通信之前必须建立连接；数据报自含报文，发送数据类似于寄邮件，不能保证次序，可能会丢失，每封信都包含接收者地址，面向连接的通信像是打电话，先要建立连接，对话中不包含地址信息。

SOCK_SEQPACKET与SOCK_STREAM很像，只是得到的是基于报文而非字节流的服务，流控制传输协议（Stream Control Transmission Protocol）提供了顺序数据包服务

SOCK_RAW用于直接访问网络层，应用程序需要自己负责构造协议头部

并非所有以文件描述符为参数的函数都可以接受套接字描述符，未指定和与实现相关通常对套接字描述符无效：

```
close: 释放套接字
dup / dup2: 复制套接字
fchdir: 失败，并且将errno设置为ENOTDIR
fchomod: 未指定
fchown: 由实现定义
fcntl: 支持某些命令
fdatasync / fsync: 由实现定义
fstat: 支持一些stat结构成员，如何支持由实现定义
ftruncate: 未指定
ioctl: 依赖于底层设备驱动
lseek: 由实现定义，失败时将errno设置为ESPIPE
mmap: 未指定
poll: 正常工作
pread / pwrite: 失败时，将errno设置为ESPIPE
read / readv: 与没有任何标志位的recv等价
select: 正常工作
write / writev: 与没有任何标志位的send等价
```

套接字是双向的，可以禁止其IO：

```
int shutdown(int sockfd, int how);
// 若成功，返回0，若出错，返回-1

close可以关闭一个套接字，但是只有最后一个活动引用关闭时，close才会释放网络端点，而shutdown允许一个套接字处于不活动状态，和引用它的文件描述符数目无关，还可以方便的关闭双向传输中的一个方向，如读或写
```



