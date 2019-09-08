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

##### 消息队列

消息的链接表，存储在内核，由消息队列标识符标识，不一定以先进先出顺序取消息，可以按消息的类型字段读

```
struct msqid_ds 
{
    struct ipc_perm msg_perm;	/* see Section 15.6.2 */
    struct msg *msg_first;      /* first message on queue,unused  */
    struct msg *msg_last;       /* last message in queue,unused */
    __kernel_time_t msg_stime;  /* last msgsnd time */
    __kernel_time_t msg_rtime;  /* last msgrcv time */
    __kernel_time_t msg_ctime;  /* last change time */
    unsigned long  msg_lcbytes; /* Reuse junk fields for 32 bit */
    unsigned long  msg_lqbytes; /* ditto */
    unsigned short msg_cbytes;  /* current number of bytes on queue */
    unsigned short msg_qnum;    /* number of messages in queue */
    unsigned short msg_qbytes;  /* max number of bytes on queue */
    __kernel_ipc_pid_t msg_lspid;   /* pid of last msgsnd */
    __kernel_ipc_pid_t msg_lrpid;   /* last receive pid */
};
```

创建新队列或打开现有队列：

```
#include <sys/msg.h>

int msgget(key_t key, int flag);
// 若成功，返回消息队列ID，若出错，返回-1

flag代表的是权限
用户读        0400
用户写(更改)   0200
组读          0040
组写(更改)     0020
其他读         0004
其他写(更改)    0002

如果创建新队列，需要初始化msqid_ds的以下成员：
msg_perm：该结构中的mode成员按照flag的权限位设置
msg_qnum、msg_lspid、msg_lrpid、msg_stime、msg_rtime为0
msg_ctime：设置为当前时间
msg_qbytes：设置为系统限制值
```

msgctl可以对队列执行多种操作：

```
int msgctl(int msqid, int cmd, struct msgid_ds *buf);
// 若成功，返回0，若出错，返回-1

cmd参数指定对msgid的队列要执行的命令，这三条命令也适用于信号量和共享存储
IPC_STAT：取队列的msqid_ds结构，放在buf中
IPC_SET：将字段msg_perm.uid、msg_perm.gid、msg_perm.mode和msg_qbytes从buf指向的结构复制到msqid
IPC_RMID：删除消息队列及其数据，立刻生效
```

将消息添加到消息队列中：

```
int msgsnd(int msqid, const void *ptr, size_t nbytes, int flag);
// 若成功，返回0，若出错，返回-1

每个消息包含三部分：一个正的长整型字段、一个非负长度nbytes以及实际数据，总是放在队列尾端
flag：可以指定为IPC_NOWAIT，类似于文件IO的非阻塞标志
msgsnd若成功返回，消息队列相关的msqid_ds结构会随之更新

ptr一般可定义为：
struct mymesg
{
    long mtype;
    char mtext[512];
}
```

从队列中读取消息：

```
ssize_t msgrcv(int msqid, void *ptr, size_t nbytes, long type, int flag);
// 若成功，返回消息数据部分的长度，若出错，返回-1

和msgsnd一样，ptr指向一个长整型数，其后是实际消息数据的缓冲区
nbytes：缓冲区的长度
flag：若消息长度大于nbytes，且flag设置了MSG_NOERROR，消息会被截断，不设置此标志位，则出错返回E2BIG，也可以设置MSG_NOWAIT
type：返回哪一种消息，非0来取非先进先出的消息
    type==0：返回队列第一条消息
    type>0：返回队列消息类型是type的第一条消息
    type<0：返回消息队列中消息类型值小于等于type绝对值的消息，如果由多个，取类型值最小的消息
```

##### 信号量

 一个计数器，用于为多个进程提供对共享对象的访问，为了获取共享资源，进程执行如下操作：

1. 测试控制该资源的信号量
2. 若信号量为正，则进程使用该资源，信号量值减1
3. 若信号量为0，则进程进入休眠状态，直至信号量值大于0，进程被唤醒，返回步骤1

进程不再使用由信号量控制的共享资源时，信号量值增1，如果有进程正在休眠等待此信号量，唤醒它们

这种形式的信号量为二元信号量，一般而言，初始值可以是任意正值，表示有多少个共享资源可供使用，信号量通常内核实现。XSI信号量要复杂得多，因为三个特性：

1. 信号量并非是单个非负值，而是含有一个或多个信号量值得集合
2. 信号量的创建独立于初始化，不能原子性的创建一个信号量集合并对其赋值
3. 即使没有使用各种形式的XSI IPC，它们仍然是存在的，有的程序终止时没有释放已经分配的信号量

```
struct semid_ds 
{
    struct ipc_perm    sem_perm;        /* 对信号操作的许可权 */
    __kernel_time_t    sem_otime;        /*对信号操作的最后时间 */
    __kernel_time_t    sem_ctime;        /*对信号进行修改的最后时间 */
    struct sem    *sem_base;        /*指向第一个信号 */
    struct sem_queue *sem_pending;        /* 等待处理的挂起操作 */
    struct sem_queue **sem_pending_last;    /* 最后一个正在挂起的操作 */
    struct sem_undo    *undo;            /* 撤销的请求 */
    unsigned short    sem_nsems;        /* 数组中的信号数 */
};
```

获取一个信号量ID：

```
#include <sys/sem.h>

int semget(key_t key, int nsems, int flag);
// 若成功，返回信号量ID，若出错，返回-1

创建一个新集合，必须指定nsems，引用现有集合，将nsems指定为0
```

包含了多种操作的semctl：

```
int semctl(int semid, int semnum, int cmd, .../* union semun arg */);
// 返回值，见下

arg是联合，而非联合的指针
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}

cmd的含义如下：
IPC_STAT：对此集合取semid_ds结构，存储在arg.buf指向的结构
IPC_SET：对信号量的属性进行设置
IPC_RNID：删除semid指定的信号集
GETPID：返回最后一个执行semop操作的进程ID
GETVAL：返回信号集semnum指定信号的值
GETALL：返回信号集中所用信号的值
GETNCNT：返回正在等待资源的进程的数量
GETZCNT：返回正在等待完全空闲资源的进程的数量
SETVAL：设置信号集中semnum指定的信号的值
SETALL：设置信号集中所用信号的值

除了GETALL以外的所有GET命令，返回相应值，其他命令，若成功，返回0，若出错，返回-1
```

自动执行信号量集合上的操作数组：

```
int semop(int semid, struct sembuf semoparray[], size_t nops);
// 若成功，返回0，若出错，返回-1

具有原子性，或者执行数组中所有操作，或者一个也不做

semoparray指向一个有sembuf结构表示的信号量操作数组
struct sembuf
{
    unsigned short sem_num;
    short sem_op;	/* operatioin(negative, 0, or positive */
    short sem_flg;	/* IPC_NOWAIT, SEM_UNDO */
}

sem_op为正：表示进程释放的占用的资源数，sem_op的值会加到信号量值上，若指定了SEM_UNDO，则从信号量调整值减去sem_op
sem_op为负：表示要获取由该信号量控制的资源，若信号量值大于等于sem_op绝对值，则从信号量值中减去sem_op绝对值，若指定了SEM_UNDO，则sem_op的绝对值加到信号量调整值上
sem_op为0：表示调用进程希望等待到该信号量值变成0
```

无论何时只要为信号量操作指定SEM_UNDO标志，再分配资源（sem_op小于0），内核就会记住对于该特定信号量分配给了进程多少资源，进程终止时，内核会检测该进程是否还有尚未处理的信号量调整值，如果有按照调整值进行处理，如果用带SETVAL或SETALL命令的semctl设置一个信号量的值，则在所有进程中，该信号量的调整值被设置为0

##### 共享存储

允许两个或多个进程共享一个给定的存储区，因为数据不需要在客户进程和服务器进程之间复制，这是最快的一种IPC

```
struct shmid_ds 
{
    struct ipc_perm        shm_perm;    /* 操作许可 */
    int            shm_segsz;    /* 共享内存大小，字节为单位 */
    __kernel_time_t        shm_atime;    /* 最后一个进程访问共享内存的时间 */
    __kernel_time_t        shm_dtime;    /* 最后一个进程离开共享内存的时间 */
    __kernel_time_t        shm_ctime;    /* 最后一次修改共享内存的时间 */
    __kernel_ipc_pid_t    shm_cpid;    /* 创建共享内存的进程ID */
    __kernel_ipc_pid_t    shm_lpid;    /* 最后操作共享内存的进程ID */
    unsigned short        shm_nattch;    /* 当前使用该贡献内存的进程数量 */
    unsigned short         shm_unused;    /* compatibility */
    void             *shm_unused2;    /* ditto - used by DIPC */
    void            *shm_unused3;    /* unused */
};
```

获取一个共享存储标识符：

```
#include <sys/shm.h>

int shmget(key_t key, size_t size, int flag);
// 若成功，返回共享存储ID，若出错，返回-1

size是共享存储段的长度，如果创建，必须指定size，且其内容初始化为0，如果引用，则将size指定为0
```

可对共享存储执行多种操作：

```
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
// 若成功，返回0，若出错，返回-1

cmd的含义如下：
IPC_STAT：读取共享内存区的shmid_ds机构，并将其存储到buf指向的地址
IPC_RMID：从系统中删除由shmid指向的共享内存区
IPC_SET：设置共享内存的shmid_ds结构

Linux和Solaris还提供了另外两个命令
SHM_LOCK：对共享存储加锁
SHM_UNLOCK：对共享存储解锁
```

创建之后，需要将其连接到地址空间：

```
void *shmat(int shmid, const void *addr, int flag);
// 若成功，返回指向共享存储段的指针，若出错，返回-1

addr==0：内核选择的第一个可用地址上，推荐的方式
addr非0且没有指定SHM_RND：连接到addr指定的地址(SHM_RND的意思是取整)
addr非0且指定了SHM_RND：连接到addr mod SHMLBA所表示的地址

flag指定为SHM_RDONLY表示只读，否则以读写方式连接
```

操作完成后，需要与其分离：

```
int shmdt(const void *addr);
// 若成功，返回0，若出错，返回-1

addr是shmat的返回值
```

但是标识符依然存在，直到带IPC_RMID命令的shmctl删除它

##### POSIX信号量

意在解决XSI信号量的几个缺陷：

* 考虑了更高性能的实现
* 接口更简单，没有信号量集
* 删除信号量时表现更完美

未命名信号量：只存在于内存中，要求能使用信号量的进程必须可以访问内存

命名信号量：通过名字访问，可用于不同进程的线程

创建新的或引用现有信号量：

```
#include <semaphone.h>

sem_t *sem_open(const char *name, int oflag, .../* mode_t mode, unsigned int value */);
// 若成功，返回指向信号量的指针，若出错，返回SEM_FAILED

如果引用：需要指定name和oflag为0，无需指定mode
如果创建：当oflag指定为O_CREATE，需要提供两个额外参数，mode指定谁可以访问信号量，取值与打开文件权限位相同，value用来指定信号量初始值

信号量命名规则：
1. 第一个字符应该为斜杠/
2. 不应包含其他斜杠以此避免实现定义的行为
3. 最大长度是实现定义，不应超过_POSIX_NAME_MAX
```

释放信号量相关的资源：

```
int sem_close(sem_t *sem);
// 若成功，返回0，若出错，返回-1

如果进程没有调用sem_close而退出，内核将自动关闭任何打开的信号量，但不会影响信号量值的状态
```

销毁一个命名信号量：

```
int sem_unlink(const char *name);
// 若成功，返回0，若出错，返回-1

如果最后一个打开的引用关闭了，则删除信号量的名字
```

信号量的减一操作：

```
int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict tsptr); // 信号量计数为0，先等待，过了时间计数没能减一，返回-1
int sem_trywait(sem_t *sem); // 信号量计数为0，不会阻塞，返回-1
int sem_wait(sem_t *sem);  // 信号量计数为0，则阻塞直到成功使信号量减一或被信号中断
// 三个函数返回值：若成功，返回0，若出错，返回-1
```

信号量的加一操作：

```
int sem_post(sem_t *sem);
// 若成功，返回0，若出错，返回-1

// 调用sem_post时，如果调用sem_wait发生阻塞，进程会被唤醒且被sem_post加一的信号量计数会再次被sem_wait减一
```

未命名信号量的创建：

```
int sem_init(sem_t *sem, int pshared, unsigned int value);
// 若成功，返回0，若出错，返回-1

pshared：表示是否在多个进程使用信号量，如果是，将其设置为非0值
value：指定了信号量的初始值
```

未命名信号量的销毁：

```
int sem_destroy(sem_t *sem);
// 若成功，返回0，若出错，返回-1

调用之后，不能再使用任何带有sem的信号量函数，除非通过调用sem_init重新初始化
```

检索信号量的值：

```
int sem_getvalue(sem_t *restrict sem, int *restrict valp);
// 若成功，返回0，若出错，返回-1

如果成功，valp返回信号量的值，在读出来的时候可能已经变了，仅用于调试
```

使用管道和FIFO，尽可能避免使用消息队列以及信号量，应该考虑全双工管道和记录锁，它们使用起来简单的多，共享存储仍然有其用途，虽然通过mmap函数也能提供同样的功能