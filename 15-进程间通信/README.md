### 第十五章  进程间通信

##### 引言

```
IPC类型

半双工管道
FIFO
全双工管道
命名全双工管道
XSI信号量
XSI消息队列
XSI共享存储
消息队列(实时)
信号量
共享存储(实时)
套接字
STREAMS
```

##### 管道

局限：

* 历史上，实现为半双工，虽然有些系统提供全双工管道
* 只能在具有公共祖先的两个进程之间使用，通常是在fork之后的父进程和子进程之间使用

FIFO没有第二种局限，域套接字没有这这两种局限，尽管如此，半双工管道仍然是最常用的IPC形式

```
#include <unistd.h>

int pipe(int fd[2]);	
// 若成功，返回0，若出错，返回-1
```

fd[0]为读打开，fd[1]为写打开，fd[1]的输出是fd[0]的输入

单个进程的管道几乎没有任何作用，通常，进程先调用pipe，再fork，从而创建从父进程到子进程的IPC通道；从父进程->子进程的管道，父进程关闭读端，子进程关闭写端，从子进程->父进程的管道，则相反；当管道的一端被关闭后，以下两条规则起作用：

1. 当读一个写端已被关闭的管道时，所有数据被读取后，read返回0，表示文件结束
2. 当写一个读端已被关闭的管道时，则产生信号SIGPIPE，若忽略此信号或从信号处理函数返回，write返回-1，errno设置为EPIPE

常量PIPE_BUF规定了内核的管道缓冲区大小，可以用pathconf或fpathconf确定

##### 函数popen和pclose

常见的操作时创建一个文件连接到另一个进程的管道，然后读其输出或向其输入端发送数据，popen和pclose实现的操作是：创建一个管道，fork一个子进程，关闭未使用的管道端，执行一个shell运行命令，然后等待命令终止

```
#include <stdio.h>

FILE *popen (const char *cmdstring , const char * type);
// 若成功，返回文件指针，若出错，返回NULL
int pclose (FILE *fp);
// 若成功，返回cmdstring的终止状态，若出错，返回-1
```

* popen先执行fork，然后调用exec执行cmdstring，并且返回一个标准IO文件指针，如果type是r，则文件指针连接到cmdstring的标准输出，如果type是r，则文件指针连接到cmdstring的标准输入

* pclose关闭标准IO流，等待命令终止，然后返回shell的终止状态

##### 协同进程

当一个过滤程序既产生某个过滤程序的输入，又读取过滤程序的输出时，就变成了协同进程，Korn shell提供了协同进程，而其他shell没有提供；通常在shell的后台运行，其标准输入和标准输出通过管道连接到另一个程序；popen只提供连接到另一个进程的标准输入或标准输出的单向管道，而协同进程则连接到另一个进程的两个单向管道：一个连接到其标准输入，另一个则来自其标准输出，我们将数据写到其标准输入，处理后，再从其标准输出读取数据

##### FIFO

有时候被称为命名管道，是因为未命名的管道只能在两个相关的进程之间使用，而且这两个进程要有一个共同的祖先进程，但是FIFO，不相关的进程也能交换数据；FIFO是一种文件类型，通过stat结构的st_mode成员编码可以直达是否FIFO类型，可以用S_ISFIFO进行测试

```
#include <sys/stat.h>

int mkfifo(const char *path, mode_t mode);
int mkfifoat(int fd, const char *path, mode_t mode);
// 两个函数返回值：若成功，返回0，若出错，返回-1
```

mode参数与open中的mode相同，FIFO有两种用途：

1. shell命令使用FIFO将数据从一条管道传送到另一条时，无需创建临时文件
2. 客户进程-服务器进程应用程序中，FIFO用作汇聚点，在两者之间传递数据

* 用FIFO复制数据流：管道只能用于两个进程之间的线性连接，而FIFO可用于非线性连接，如果需要对一个经过过滤的输入流进行两次处理，可以使用FIFO和tee实现而无需临时文件：创建FIFO，后台启动prog3，从FIFO读取数据，然后启动prog1，用tee将其输出发送到FIFO和prog2

```
mkfifo fifo1
prog3 < fifo1 &
prog1 < infile | tee fifo1 | prog2
```

* 用FIFO进行客户进程和服务器进程通信

##### XSI IPC

三种XSI IPC：信号量、消息队列和共享存储器，它们有很多相似之处；无论何时创建IPC结构（msgget、memget、shmget），都应该指定一个键，类型是key_t，有多重方法使得客户进程和服务器进程在同一IPC上汇聚：

* 服务器进程指定IPC_PRIVATE创建一个新的IPC结构，缺点是文件系统操作需要服务器进程将返回值写到文件供客户进程读取
* 在一个公共文件定义一个客户进程和服务器进程都认可的键，然后服务器进程根据此键创建新的IPC结构，缺点是需要判断该键是否已经使用
* 客户进程和服务器进程认同的路径名和项目ID（0-255之间），接着调用ftok将这两个值变换为一个键，然后用第二种方法

```
#include <sys/ipc.h>

key_t ftok(const char *path, int id);  // path必须引用现有的一个文件
// 若成功，返回键，若出错，返回-1
```

IPC_PRIVATE总是创建一个新的IPC结构，同时指定IPC_CREATE和IPC_EXCL，如果IPC结构已经存在，则出错返回EEXIST；如果要引用一个IPC结构，key必须是创建时指明的key的值，并且不指定IPC_CREATE

IPC的权限结构：

```
struct ipc_perm
{
key_t        key;                        调用shmget()时给出的关键字
uid_t           uid;                      /*共享内存所有者的有效用户ID */
gid_t          gid;                       /* 共享内存所有者所属组的有效组ID*/ 
uid_t          cuid;                    /* 共享内存创建 者的有效用户ID*/
gid_t         cgid;                   	/* 共享内存创建者所属组的有效组ID*/
unsigned short   mode;    				/* Permissions + SHM_DEST和SHM_LOCKED标志*/
unsignedshort    seq;          			/* 序列号*/
};
```

可以调用msgctl、semctl或shmctl修改uid、gid和mode字段，但必须是IPC结构的创建者或超级用户；消息队列和共享存储器使用术语“读”或“写”，信号量使用术语“读”或“更改”；当最后一个引用管道的进程终止时，管道就被完全的删除了，对于FIFO而言，在最后一个引用FIFO的进程终止时，虽然FIFO的名字仍然保留在系统直到被显式的删除，但是留在FIFO中的数据已经被删除了；由于IPC不使用文件描述符，不能对它们使用多路转换

