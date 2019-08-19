### 第十四章  高级I/O

##### 非阻塞IO

系统调用分为低速系统调用和其他，低速调用可能会使进程永远阻塞，包括：

* 如果文件不存在，读操作
* 如果数据不能被立即接受，写操作
* 某些条件发生前打开某些类型的文件（管道中无空间，网络流控制）
* 对已经加强制记录锁的文件进行读写
* 某些ioctl操作
* 某些进程间通信函数

对于一个文件描述符，有两种方法指定为非阻塞

1. 如果调用open，可指定O_NONBLOCK
2. 如果已经打开，可调用fcntl，由该函数打开O_NONBLOCK

##### 记录锁

当第一个进程正在读或写文件的某个部分时，使用记录锁可以阻止其他进程修改同一文件区，记录即文件的一个区域或整体

```
#include<fcntl.h>  
int fcntl(int fd, int cmd ,struct flock* lock);
// 若成功，依赖cmd，若出错，返回-1

struct flock
{
    short l_type;    /*锁的类型：F_RDLCK,F_WRLCK,F_UNLCK*/
    short l_whence;  /*偏移量的起始位置：SEEK_SET,SEEK_CUR,SEEK_END*/
    off_t l_start;   /*加锁的起始偏移，0且l_whence为SEEK_SET且l_len为0表示对整个文件加锁*/
    off_t l_len;     /*上锁字节：0表示锁的范围扩展到最大可能偏移量*/
    pid_t l_pid;     /*锁的属主进程ID */
}; 
```

加读锁时，必须读打开，加写锁时，必须写打开，cmd的三种类型：

* F_GETLK：判断由lock描述的锁是否会被另外一把锁排斥（阻塞）
* F_SETLK：设置由lock描述的锁
* F_SETLKW：F_SETLK的阻塞版本，W表示wait

设置或释放文件上的一把锁时，系统会组合或分裂相邻区，如100-199字节加锁区，需解锁150字节，内核将维持两把锁，一100-149字节，二151-199字节，如果再对150字节加锁，系统会把三个相邻的加锁区合并成一个

记录锁的自动继承和释放的规则：

1. 当一个进程终止时，它建立的所有锁全部释放；无论一个描述符何时关闭，该进程通过这一描述符引用的文件上的任何一把锁全部释放
2. 由fork产生的子进程不继承父进程设置的锁
3. 在执行exec后，新程序可以继承原执行程序的锁，除非该文件未设置执行关闭标志

##### IO多路转换

从一个描述符读，又写到另一个描述符，可以使用阻塞IO：

```
while ((n = read(STDIN_FILENO, buf, BUFSIZ)) > 0)
    if (write(STDOUT_FILENO, buf, n) != 0)
        perror("write error");
```

如果要从两个描述符读，就不能在任一描述符上阻塞读，否则导致另一个描述符即使有数据也无法处理。

观察telnet命令：从终端标准输入读，将数据写到网络连接上，同时从网络连接读，将数据写到终端标准输出；而在网络连接的另一端，telnetd守护进程读取用户输入的命令，送给shell处理，再将输出传给用户。

* 一个方法是将一个进程变成两个进程（fork），每个进程处理一条数据通路，但是无法确定操作什么时候终止

* 一个方法是使用一个进程的两个线程，虽然避免了终止的复杂性，但要处理线程同步
* 一个方法是仍旧使用一个进程执行该程序，但使用非阻塞IO读取数据，即所谓轮询，但是浪费CPU
* 一个方法是异步IO，但存在可移植性问题
* 比较好的处理方法是使用IO多路转换：先构造一张描述符的列表，调用一个函数，直到这些描述符中的一个已经准备好IO时，该函数才返回，poll、pselect和select这三个函数可以执行IO多路转换

```
#include <sys/select.h>

int select(int maxfdp1, fd_set *restrict readfds, fd_set *restrict writefds,
            fd_set *restrict exceptfds, struct timeval *restrict tvptr);
// 返回值：准备就绪的描述符数目，若超时，返回0，若出错，返回-1
```

最后一个参数表示愿意等待的时间长度

* tvptr==NULL：永远等待，当捕捉到指定描述符中的一个已准备好或捕捉到一个信号则返回
* tvptr.tv_sec==0 && tvptr.tv_usec==0：不等待，立刻返回
* tvptr.tv_sec!=0 || tvptr.tv_usec!=0：等待的秒数和微秒数，时间未到，可像第一种条件被打断

中间三个参数是指向可读、可写、处于异常条件的描述符集的指针，都可以是空指针，表示不关心此条件，若三个都是空指针，则select提供了比sleep更精确的定时器

```
void FD_ZERO(fd_set *fdset);          // 清空集合
void FD_SET(int fd, fd_set *fdset);   // 将一个给定的文件描述符加入集合之中
void FD_CLR(int fd, fd_set *fdset);   // 将一个给定的文件描述符从集合中删除
int FD_ISSET(int fd, fd_set *fdset);  // 若fd在描述符集中，返回非0，否则，返回0
```

第一个参数maxfdp1是最大文件描述符的值加1，也可设置为FD_SETSIZE，这值太大了，大多数程序只使用3-10个描述符

对于描述符“准备就绪”的含义：

* 对读集中的一个描述符进行read不会阻塞，认为此描述符是准备就绪的
* 对写集中的一个描述符进行write不会阻塞，认为此描述符是准备就绪的
* 对异常条件集中的一个描述符有一个未决异常条件，认为此描述符是准备就绪的
* 对读、写、异常条件，普通文件的文件描述符总是返回准备就绪

一个描述符阻塞并不影响select是否阻塞

```
#include <sys/select.h>

int pselect(int maxfdp1, fd_set *restrict readfds, fd_set *restrict writefds,
fd_set *restrict exceptfds, const struct timespec *restrict tsptr
const sigset_t *restrict sigmask);
// 返回值：准备就绪的描述符数目；若超时，则返回0；若出错，返回-1
```

* 超时值使用timespec结构，以秒和纳秒表示超时值，能够提供更精确的超时时间，超时值被声明为const，这保证了超时值不会在调用时被修改
* 使用可选的信号屏蔽字，如果参数sigmask非空，则在函数被调用时，以原子的方式安装该屏蔽字，再返回时，恢复以前的屏蔽字

与select不同，poll构造一个pollfd的数组，每个数组元素指定一个描述符编号以及我们对该描述符感兴趣的条件

```
#include <poll.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
// 返回值：准备就绪的描述符数目，若超时，返回0，若出错，返回-1

// pollfd结构
struct pollfd
{
    int fd;          /* file descriptor */
    short events;    /* requested events*/
    short revents;   /* returned events*/
};
```

最后一个参数表示愿意等待的时间长度

* timeout==-1：永远等待，当指定的一个描述符准备就绪或捕捉到一个信号则返回
* timeout==0：不等待，立刻返回
* timeout>0：等待的毫秒数

中间参数nfds表示数组fds的长度

第一个参数的events和revents

```
POLLIN 　　　　　　　有数据可读
POLLRDNORM 　　　　 有普通数据可读
POLLRDBAND　　　　　有优先数据可读
POLLPRI　　　　　　　有紧迫数据可读
POLLOUT　　　　　　 写数据不会导致阻塞
POLLWRNORM　　　　　写普通数据不会导致阻塞
POLLWRBAND　　　　　写优先数据不会导致阻塞
POLLMSGSIGPOLL 　　消息可用

revents还可以是：
POLLER　　  指定的文件描述符发生错误
POLLHUP　　 指定的文件描述符挂起事件
POLLNVAL　　指定的文件描述符非法
```

与select一样，一个描述符是否阻塞不会影响poll是否阻塞

##### 异步IO

POSIX异步IO接口为不同类型的文件进行异步IO提供了一套一致的方法，这些接口使用AIO控制块来描述IO操作

```
#include <aiocb.h>

struct aiocb {
  /* 下面所有字段依赖于具体实现 */
  int             aio_fildes;     /* 文件描述符 */
  off_t           aio_offset;     /* 文件偏移 */
  volatile void  *aio_buf;        /* 缓冲区地址 */
  size_t          aio_nbytes;     /* 传输的数据长度 */
  int             aio_reqprio;    /* 请求优先级 */
  struct sigevent aio_sigevent;   /* 通知方法 */
  int             aio_lio_opcode; /* 仅被 lio_listio() 函数使用 */

  /* Various implementation-internal fields not shown */
};
```

不能在同一个进程中把异步IO函数和传统IO函数混在一起操作同一个文件

```
struct sigevent {
    int           sigev_notify;            /* 通知类型 */
    int           sigev_signo;             /* 信号编号 */ 
    union sigval  sigev_value;             /* 信号数值 */ 
    void         (*sigev_notify_function)(union sigval); /* 通知函数 */ 
    pthread_attr_t *sigev_notify_attributes;  /* 通知属性 */ 
}; 
```

sigev_notify字段控制通知类型，可能是以下三者之一：

* SIGEV_NONE：异步IO请求完成后，不通知进程
* SIGEV_SIGNAL：异步IO请求完成后，产生由sigev_signo字段指定的信号
* SIGEV_THREAD：异步IO请求完成后，由sigev_notify_function字段指定的函数被调用

