### 第八章  进程控制

##### 进程标识

* 进程ID虽然唯一但可复用，大多系统采用了延迟复用算法，使得新建进程ID不同于最近终止进程ID，防止将新进程误认为是同一ID的某个已经终止的进程

* ID为0：调度进程或交换进程，是内核的一部分，并不执行磁盘上的程序
* ID为1：通常是init进程，由内核调用，绝不会终止。一般是/etc/init或/sbin/init，在Mac中是launchd

```
#include <unistd.h>
pid_t getpid(void);	// 返回进程id
pid_t getppid(void); // 返回父进程id
pid_t getuid(void); // 返回实际用户id
pid_t geteuid(void); // 返回有效用户id
pid_t getgid(void); // 返回实际组id
pid_t getegid(void); // 返回有效组id
```

##### 函数fork

现有进程可以调用fork创建一个新进程

```
#include <unistd.h>
pid_t fork(void);
// 子进程返回0，父进程返回子进程ID，出错返回-1
```

* fork创建的新进程为子进程
* fork调用一次，返回两次。将子进程ID返回给父进程的理由是子进程可以有多个，而没有接口可以全部获取，而子进程返回0的理由是一个子进程的ID不可能是0
* 子进程获取到父进程的数据空间、堆和栈副本，共享正文段
* fork之后父进程还是子进程先执行是不确定的，取决于内核调度算法
* 如果需要父进程和子进程之间同步，需要某种形式的进程间通信

父进程和子进程每个相同的打开描述符共享一个文件表项和文件偏移量，如果没有同步，它们的输出就会相互混合，但这不是常见的操作模式，fork之后处理文件描述符有两种常见情况：

* 父进程等待子进程完成
* 父进程和子进程各自执行不同的程序段

除了打开文件，父进程的很多属性也有子进程继承：

* 实际用户ID、实际组ID、有效用户ID、有效组ID，附属组ID
* 进程组ID、会话ID
* 控制终端
* 设置用户ID标志和设置组ID标志
* 当前工作目录
* 根目录
* 文件模式创建屏蔽字
* 信号屏蔽和安排
* 对任意打开文件描述符的执行时关闭标志
* 环境
* 存储映像
* 资源限制

父进程和子进程的区别：

* fork的返回值
* 进程ID
* 父进程ID
* 子进程不继承父进程设置的文件锁
* 子进程的未处理闹钟被清除
* 子进程的未处理信号集设置为空集
* 子进程的tms_utime、tms_stime、tms_cutime和tms_ustime设置为0

fork失败的原因：

* 系统中有太多的进程
* 该实际用户ID的进程总数（CHILD_MAX）超过了系统限制

fork有两种常见用法：

* 网络常见用法：父进程和子进程同时执行不同的代码段，父进程等待客户端的请求，请求到达时，fork子进程处理此请求，父进程继续等待下一个服务请求
* shell常见用法：调用exec执行另一个程序

##### 函数vfork

用于创建一个新进程，而该新进程的目的是exec一个新程序，它与fork的区别：

* vfork不会把父进程的地址空间完全复制到子进程，因为子进程会马上exec，不会引用该地址空间
* vfork保证子进程先运行，在它调用exec或exit后父进程才可能被调度

##### 函数exit

五种正常退出方式：

* main中执行return，等效于exit
* 直接调用exit，调用终止处理程序，关闭所有标准IO
* 调用  _exit  或   _Exit，目的是为进程提供一种无需运行终止处理程序或信号处理程序而终止的方法，对标准IO释放冲洗，取决于实现。 _exit 或 _Exit是同义的，而 _exit则由exit调用

* 进程最后一个线程执行return，进程以终止状态0返回
* 进程最后一个线程调用pthread_exit函数，如上，进程以终止状态0返回

三种异常退出方式：

* 调用abort，产生SIGABRT信号，这是下一种情况的特例
* 进程接收到某些信号时（信号可由进程自身如调用abort，其他进程或内核产生），内核就会为该进程产生相应信号
* 最后一个线程对取消作出响应

不管进程如何终止，都应该通知父进程它是如何终止的，将其“退出状态”作为参数传给函数，退出时，内核将“退出状态”转化为“终止状态”，父进程能用wait或waitpid取得该”终止状态“。对于父进程已经终止的进程，它们的父进程都改变为init进程，称被init收养，保证每个进程都有一个父进程。一个已经终止、但是其父进程未对其进行善后处理（获取终止子进程有关信息：进程ID、终止状态、使用的CUP时间总量等，释放它仍占用的资源）的进程称为僵尸进程（zombie），除非父进程等待取得了子进程的终止状态，不然子进程终止后会变成僵尸进程。init的子进程包括被收养的进程，如果终止了，init就会调用wai取得其终止状态，这样防止了系统中塞满僵尸进程

##### 函数wait和waitpid

当一个进程正常或异常终止时，内核就向父进程发送SIGCHLD信号，系统的默认动作时忽略它，如果调用wait或waitpid则：

* 如果其所有子进程都在运行，则阻塞
* 如果一个子进程已经终止，正等待父进程获取其终止状态，则取得该子进程的终止状态立即返回
* 如果没有任何子进程，则出错返回

```
#include <sys/wait.h>
pid_t wait(int *status); 
pid_t waitpid(pid_t pid, int *status, int options);
// 两个函数，若成功，返回进程ID，若出错，返回0或-1
```

```
以下四个互斥的宏可以取得进程终止的原因：
WIFEXITED(status)：子进程正常结束则为真，可以通过WEXITSTATUS(status)取得子进程exit()返回的结束代码
WIFSIGNALED(status)：异常终止子进程则为真，可以通过WTERMSIG(status)取得子进程因信号而中止的信号代码
WIFSTOPPED(status)：子进程处于暂停状态则为真，可以通过WSTOPSIG(status)取得引发子进程暂停的信号代码
WIFCONTINUED(status)：在作业控制暂停后已经继续的子进程返回则为真
```

两个函数的区别：

* 在一个子进程终止前，wait使其阻塞，而waitpid有一选项，可使其不阻塞
* waitpid并不等待第一个终止子进程，它有若干选项，可以控制所等待的子进程，提供了三个wait没有的功能
  * waitpid可等待一个特定的进程，而wait则返回任一终止子进程的状态
  * waitpid提供了wait的非阻塞版本，有时候希望获取子进程的状态，而不阻塞
  * waitpid通过WUNTRACED和WCONTINUED选项支持作业控制

参数说明：

* status如果为空，表示不关心终止状态，如果不为空，则保存终止状态
* waitpid的第一个参数说明
  * pid=-1：等待任一子进程，等同于wait
  * pid=>0：等待进程ID与pid相等的子进程
  * pid==0：等待组ID等于调用进程组ID的任一子进程
  * pid<-1：等待组ID等于pid绝对值的任一子进程

* option参数说明：

```
WNOHANG		若由pid指定的子进程未发生状态改变(没有结束)，则waitpid()不阻塞，立即返回0
WUNTRACED	若支持作业控制，返回终止子进程信息和因信号停止的子进程信息
WCONTINUED	若支持作业控制，返回收到SIGCONT信号而恢复执行的已停止子进程状态信息
```

##### 函数waitid

类似于waitpid，但提供了更多的灵活性

```
#include <sys/wait.h>
pid_t waitid(idtype_t idtype, it_t, id, siginfo_t, *infop, int options);
// 若成功，返回0，若出错，返回-1
```

使用两个单独的参数表示要等待的子进程所属的类型，id的作用和idtype有关，idtype的类型：

```
P_PID	等待一个特定的进程：id包含要等待子进程的进程ID
P_PGID	等待一个特定进程组中的任一子进程：id包含要等待子进程的进程组ID
P_ALL	等待任一子进程：忽略id
```

option的状态：

```
WCONTINUED	等待一个进程，它以前曾被暂停，此后又已继续，但其状态尚未报告
WEXITED		等待已退出的进程
WNOHANG		如无可用的子进程退出状态，立即返回而非阻塞
WNOWAIT		不破坏子进程退出状态。该子进程退出状态可由后续的wait、waitid或waitpid调用取得
WSTOPPED	等待一个进程，它已经暂停，但其状态尚未报告
```

##### 函数wait3和wait4

它们提供的功能比wait、waitpid、waitid要多一个，与附加参数有关，该参数允许内核返回由终止进程及所有子进程使用的资源概括

```
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
 
pid_t wait3(int *status, int options,
            struct rusage *rusage);
pid_t wait4(pid_t pid, int *status, int options,
            struct rusage *rusage);
// 两个函数：若成功，返回进程ID，若出错，返回-1
```

返回的资源信息包括：用户CPU时间总量、系统CPU时间总量、缺页次数、接收到信号的次数等

```
struct rusage {
                struct timeval ru_utime;
                struct timeval ru_stime;
                long   ru_maxrss;       
                long   ru_ixrss;        
                long   ru_idrss;        
                long   ru_isrss;        
                long   ru_minflt;       
                long   ru_majflt;       
                long   ru_nswap;        
                long   ru_inblock;      

                long   ru_oublock;      
                long   ru_msgsnd;       
                long   ru_msgrcv;       
                long   ru_nsignals;     
                long   ru_nvcsw;        
                long   ru_nivcsw;       
            };
```

##### 竞争条件

当多个进程都企图处理共享数据，而最后的结果取决于进程运行的顺序时，发生了竞争条件。如果一个进程希望等待一个子进程终止，必须调用wait函数中的一个，如果一个进程要等待父进程终止，可以轮询：

```
while (getppid() != -1)
	sleep(1)
```

问题是浪费了CPU资源，为了避免竞争条件和轮询，多个进程间需要某种形式的信号发送和接收的方法。主要是五个实现，TELLWAIT、TELL PARENT、TELL CHILD、WAIT PARENT、 WAIT CHILD

##### 函数exec

调用exec时，并不创建新进程，新程序完全替代原进程执行的程序，包括进程的正文段、数据段、堆栈等。通过fork创建进程、exec执行新程序、wait和exit等待终止和处理终止，这些构成了基本的进程控制原语

```
#include <unistd.h>

int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg,..., char * const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execve(const char *file, char *const argv[],char *const envp[]);
int fexecve(int fd, char *const argv[],char *const envp[]);
```

* 第一个参数是文件名、文件描述符或文件路径，当path作为参数时，如果包含/，视为路径名，否则按照PATH环境变量指定的目录搜索可执行文件
* 后缀l表示列表list，v表示矢量vector，前三个函数的每个命令行参数都是一个独立的参数，而后四个函数应先构造一个指向各参数的指针数组，再将其作为参数传递
* 以e结尾的三个函数可以传递一个指向环境字符串指针数组的指针，其他四个函数则是使用调用进程中的environ变量为新程序复制现有的环境
* 字母p表示该函数取path作为参数，并且用PATH环境变量寻找可执行文件
* l与v互斥，p与e互斥，p与f互斥

* 进程中每个打开描述符都有一个执行时关闭标志FD_CLOEXEC，若设置了此标志，执行exec时关闭该描述符
* exec执行前后实际用户ID和实际组ID保持不变，而有效ID是否改变取决于所执行程序文件的设置用户ID位和设置组ID位是否设置，如果设置了，则有效用户ID变成程序文件所有者ID，组ID处理方式一样
* 在大多UNIX实现中，只有execve是内核的系统调用，其余六个只是库函数

##### 更改用户ID和更改组ID

```
#include <unistd.h>
int setuid(uid_t uid);
int setgid(gid_t gid);
两个函数返回值，若成功，返回0，若出错，返回-1
```

* 若进程具有超级用户权限，setuid将实际用户ID、有效用户ID以及保存的设置用户ID设置为uid
* 若进程没有超级用户权限，但uid等于实际用户ID或保存的设置用户ID，则setuid只将有效用户ID设置为uid
* 若上面两个条件都不满足，则errno设置为EPEPM，返回-1

关于内核维护的三个用户ID

* 只有超级用户可以更改实际用户ID（通常是登录时由login设置，因为login是超级用户进程，当它调用setuid时同时设置三个用户ID）
* 仅当对程序文件设置了用户ID位时，exec函数才设置有效用户ID
* 保存的设置用户ID由exec函数复制有效用户ID而来

以下两个函数可以交互实际用户ID和有效用户ID的值：

```
#include <unistd.h>
int setreuid(uid_t ruid, uid_t euid);
int setregid(gid_t rgid, gid_t egid);
两个函数返回值，若成功，返回0，若出错，返回-1
```

一个非特权用户总能交互实际用户ID和有效用户ID，这就允许一个设置用户ID程序交换成用户的普通权限，以后又可以再次交换回设置用户ID的权限

以下两个函数只更改有效用户ID和有效组ID：

```
#include <unistd.h>
int seteuid(uid_t uid);
int setegid(gid_t gid);
两个函数返回值，若成功，返回0，若出错，返回-1
```

一个非特权用户可将其有效用户ID设置为其实际用户ID或保存的设置用户ID

##### 解释器文件

通常的形式：

```
! pathname[optional-argument]
```

如`! /bin/sh`，pathname通常是绝对路径

##### 函数system

```
#include <stdlib.h>
int system(const char *cmdstring);
```

如果cmdstring为空，仅当system命令可用时，返回非0值，这可以确定系统是否支持system函数，UNIX中，system总是可用的，在其实现中调用了fork、waitpid和exec，因此有三种返回值：

* fork失败或waitpid返回除了EINTR之外的出错，则system返回-1，且设置errno以指示错误类型
* 如果exec失败（表示不能执行shell），返回值如同shell执行了exit(127)一样
* 否则三个函数都成功，那么system返回shell的终止状态，格式在waitpid中已说明

使用system而不是使用fork和exec的优点：system进行了所需的各种出错处理以及各种信号处理

##### 进程会计

大多系统会以一个选项进行进程会计，启用后，每当进程结束内核就会写一个会计记录，一般包含命令名、所使用的CPU时间总量、用户ID和组ID，启动时间等。在Mac中该文件是`/var/account/acct`，在linux中该文件是`/var/account/pacct`，记录的结构定义在头文件`<sys/acct.h>`中

* 永远不能获取不终止的进程的会计记录，如init
* 会计文件中记录的顺序对应进程终止的顺序，而不是启动的顺序

会计记录对应于进程而不是程序。fork之后，内核为子进程初始化一个记录，而不是在新程序执行时初始化。虽然exec不会创建一个新的会计记录，但相应记录的命令名改变了，AFORK标志被清除。如果一个进程顺序执行了三个程序，A exec B，B exec C， C exit，只会写一个记录，该记录中的命令名是C，CPU时间是程序A、B、C之和

##### 用户标识

要找到运行程序的用户的登录名：getpwuid(getuid())，如果一个用户有多个登录名，以下函数可以拿到登录名：

```
#inlucde <unistd.h>

char *getlongin(void);
// 若成功，返回指向登录名字符串的指针，若出错，返回NULL
```

有了登录名，可用getpwnam在口令文件中查找用户的相应记录，从而确定登录shell等

##### 进程调度

调度策略和调度优先级由内核确定，进程可以通过调低nice值来降低优先级，只有特权进程允许提高调度权限，可以通过nice函数获取或更改nice值，但只能影响它所在进程

```
#include <unistd.h>

int nice(int incr);
// 若成功，返回新的nice值NZERO，若出错，返回-1
```

* nice值越小，优先级越高

* incr太大，系统会把它降到最大合法值，不给提示，incr太小，系统也会悄无声息的把它提高到最小合法值

getpriority除了可以像nice获取进程的nice值，还可以获取一组相关进程的nice值

```
#incude <sys/resource.h>

int getpriority(int which, id_t who);
// 若成功，返回-NZERO---NZERO-1之间的nice值，若出错，返回-1
```

* which：PRIO_PROCESS表示进程，which：PRIO_PGRP表示进程组，which：PRIO_USER表示用户
* 如who为0，which为PRIO_USER，表示使用调用进程的实际用户ID

* 如which作用于多个进程，返回所有作用进程中优先级最高的

setpriority可为进程、进程组和属于特定用户ID的所有进程设置优先级

```
#include <sys/resource.h>

int setpriority(int which, id_t who, int value);
// 若成功，返回0，若出错，返回-1
```

##### 进程时间

任一进程都可调用times函数获取它自己以及已终止子进程的：墙上时钟时间、用户CPU时间和系统CPU时间

```
#include <sys/times.h>
clock_t times(struct tms *buf);
// 若成功，返回流逝的墙上时钟时间(以时钟滴答数为单位)，若出错，返回-1
```

```
struct tms{
         clock_t  tms_utime;   /* user cpu time */
         clock_t  tms_stime;   /* system cpu time */
         clock_t  tms_cutime;  /* user cpu time of children */
         clock_t  tms_cstime;  /* system cpu time of children */
   };
```

函数的参数可以返回用户CPU时间和系统CPU时间，墙上时钟时间可以通过两次调用该函数的返回值之差计算
