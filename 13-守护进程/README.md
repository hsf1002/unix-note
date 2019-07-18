### 第十三章  守护进程

##### 守护进程的特征

BSD系统下ps -axj或System V系统下ps -efj查看系统进程的状态，Mac下显示如

```
UID   PID  PPID   C STIME   TTY           TIME CMD              USER              PGID   SESS JOBC STAT   TT 
    0     1     0   0 25 419  ??        31:13.67 /sbin/launchd    root                 1      0    0 Ss     ?? 
    0    46     1   0 25 419  ??         0:30.77 /usr/sbin/syslog root                46      0    0 Ss     ?? 
```

* 标题分别表示：用户ID、进程ID、父进程ID、C、STIME、终端名称、运行时长、命令字符串、用户名称、进程组ID

* 父进程为0的进程通常是内核进程，它们通常存在于系统的整个生命期内，以超级用户特权运行，无控制终端，无命令行
* 内核守护进程的名字出现在方括号内，进程1在linux下是init，在Mac下是lunchchd。linux下还有kswapd守护进程为内存换页守护进程，flush守护进程在可用内存达到最小阈值时将脏数据冲洗到磁盘，sync_seupers守护进程定期将文件系统元数据冲洗到磁盘，jbd守护进程帮助实现ext4文件系统中的日志功能，rpcbind守护进程提供将远程调用程序号映射为网络端口号，rsyslogd守护进程有管理员启动将系统消息记入日志，inetd守护进程可以侦听网络接口，cron守护进程在定期安排的时间执行命令

##### 编程规则

1. 首先要调用umask将文件模式创建屏蔽字设置为一个已知值（通常是0）
2. 调用fork，使父进程exit
3. 调用setsid创建一个新回话
4. 将当前目录更改为根目录
5. 关闭不需要的文件描述符
6. 某些守护进程需要打开/dev/null使其具有文件描述符0、1、2，因为守护进程不与终端设备关联，所以标准输入输出或标准错误都不会产生效果，既不能接收到用户输入，其输出也不会显示

##### 出错记录

BSD的syslog是一个集中的守护进程出错记录的设施，有三种方式可以产生日志：

* 内核例程调用log函数，打开并读取/dev/klog
* 大多数用户进程（守护进程）调用syslog(3)产生日志，日志被保存到/dev/log
* 不管在本机，还是通过TCP/IP网络连接到此本机的其他主机，都可将日志发向UDP端口514

syslogd守护进程读取所有三种格式的日志，此守护进程启动时读取一个配置文件/etc/syslog.conf，决定了不同种类的消息应送到何处，如紧急消息可发送到系统管理员，并在控制台打印，而警告消息可记录到文件

```
#include <syslog.h>

void openlog(const char *ident, int option, int facility);
void syslog(int priority, const char *format);
void closelog(void);
int setlogmask(int maskpri);
// 返回值：前日志记录优先级屏蔽字值
```

* openlog可选，如果不调用，在第一次调用syslog时自动调用openlog，closelog也是可选
* ident一般是程序的名称，option指定各种选项的位屏蔽
* setlogmask用于设置进程的记录优先级屏蔽字

