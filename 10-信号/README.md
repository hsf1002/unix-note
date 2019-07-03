### 第十章  信号

##### 信号概念

产生信号的条件：

* 按某些键：如DELETE或CTRL+C产生中断信号SIGINT，这是停止已经失去控制程序的办法
* 硬件异常：除零操作、无效的内存引用等，由硬件检测到通知内核
* 调用kill(2)：可将任何信号发给另一个进程或进程组
* 调用kill(1)：常用于终止一个失控的后台进程
* 软件条件：当检测到某些软件条件已经发生，应将其通知有关进程

信号是异步事件的典型事例，产生信号对进程而言是随机发生，当产生信号时，可以告诉内核按照以下三种方式之一进行处理：

* 忽略信号：大多数信号的处理方式，但是SIGKILL和SIGSTOP不能忽略，它们向内核提供了使进程停止或终止的可靠方法
* 捕捉信号：调用信号处理函数，但是不能捕捉SIGKILL和SIGSTOP
* 执行系统默认动作：大多数信号的系统默认动作是终止该进程

系统默认动作，“终止+core”表示在进程当前工作目录的core文件中复制了该进程的内存映像

```
SIGABRT    终止+core   abort
SIGALRM    终止进程     定时器超时
SIGBUS     终止+core   硬件故障
SIGCHLD    忽略        子进程状态改变
SIGHUP     终止进程     定时器超时
SIGINT     终止进程     中断进程
SIGQUIT    终止+core   终端退出符
SIGILL     终止+core       非法指令
SIGTRAP    终止+core       跟踪自陷
SIGSEGV    终止+core       段非法错误
SIGFPE     终止+core       浮点异常
SIGIOT     终止+core       执行I/O自陷
SIGKILL    终止进程     杀死进程
SIGPIPE    终止进程     向一个没有读进程的管道写数据
SIGTERM    终止进程     软件终止信号
SIGSTOP    停止进程     非终端来的停止信号
SIGTSTP    停止进程     终端来的停止信号
SIGCONT    忽略信号     继续执行一个停止的进程
SIGURG     忽略信号     I/O紧急信号
SIGLOST    终止进程     资源丢失
SIGIO      忽略信号     描述符上可以进行I/O
SIGTTOU    停止进程     后台进程写终端
SIGTTIN    停止进程     后台进程读终端
SIGXGPU    终止进程     CPU时限超时
SIGXFSZ    终止进程     文件长度过长
SIGWINCH   忽略信号     窗口大小发生变化
SIGPROF    终止进程     统计分布图用计时器到时
SIGUSR1    终止进程     用户定义信号1
SIGUSR2    终止进程     用户定义信号2
SIGVTALRM  终止进程     虚拟计时器到时
```

以下条件不产生core文件：

* 进程设置了用户ID，而当前用户并非程序文件的所有者
* 进程设置了组ID，而当前用户并非程序文件的组所有者
* 用户没有写当前工作目录的权限
* 文件太大，RLIMIT_CORE的限制

##### 函数signal

```
#include <signal.h>
void (signal(int signo, void (*func)(int)))(int);
// 若成功，返回以前的信号处理配置，若出错，返回SIG_ERR

typedef void Sigfunc(int);
Sigfunc *signal(int, Sigfunc *);
```

signal的语义与实现有关，最好使用sigaction函数代替signal函数

* signo是信号名
* func是常亮SIG_IGN（忽略此信号）、SIG_DFL（系统默认动作）或当前接到此信号后要调用的函数地址

##### 中断的系统调用

系统调用可分为低速系统调用和其他系统调用，低速系统调用是可能会使进程永远阻塞的一类系统调用，包括：

* 如果某些类型文件的数据不存在，进行读操作
* 如果数据不能被相同的类型文件立即接受，进行写操作
* 某些条件发生之前打开某些类型文件
* pause函数和wait函数
* 某些ioctl函数
* 某些进程间通信函数

除非发生硬件错误，否则IO操作总能很快返回

```
again:
    if ((n = read(fd, buf, BUFFSIZE)) < 0)
    {
        if (errno == EINTR)
        {
            goto again;
            // handle other
        }
    }
```

某些系统调用被中断后可以自动重启，包括：ioctl、read、readv、write、writev、wait和waitpid，前五个只有对低速设备进行操作时才会被信号中断，而wait和waitpid在捕捉到信号时总是被中断，某些程序可能不希望这些函数中断后重启，4.3BSD允许进程基于每个信号禁用此功能

##### 可重入函数

信号来到，进程正常指令序列临时中断，处理信号处理程序，完毕之后继续进程正常指令，但是如果正在进行malloc而被中断，且信号处理函数又进行malloc，可能会对进程造成破坏。如果在信号处理函数中保证调用安全，称为异步信号安全的。大多数函数是不可重入的，因为：

* 使用静态数据结构，如getpwnam将其结果存放到静态存储单元
* 调用malloc或free
* 标准的IO函数，如信号处理函数中的printf可能中断主程序的printf的调用

当在信号处理程序中调用可重入函数时，应该在调用前保存errno，调用后恢复errno，如经常被捕捉的信号SIGCHLD，其信号处理程序通常要调用wait函数，而各种wait函数都能改变errno

##### SIGCLD语义

Linux系统定义了SIGCLD，且其语义等于SIGCHLD，表示子进程状态改变后产生此信号，父进程需要调用一个wait函数以检测发生了什么

##### 可靠信号术语和语义

在信号产生和递送之间的时间间隔内，信号是未决的。进程可以选择“阻塞信号传递”，进程调用sigpending函数来判定哪些信号是设置为阻塞并处于未决状态的。如果在进程解除对某个信号的阻塞之前，信号发送了多次，对于大多数系统只传递一次。每个进程都有一个信号屏蔽字，规定了当前要阻塞传递到该进程的信号集，可以使用sigpromask检测和更改当前信号屏蔽字

##### 函数kill和raise

kill将信号发送给进程或进程组，raise则允许向自身发送信号

```
#include <signal.h>

int kill(pid_t pid, int signo);
int raise(int signo);
// 两个函数，若成功，返回0，若出错，返回-1

raise(signo);
等价于
kill(getpid(), signo);
```

* pid>0：将信号发送给进程ID为pid的进程
* pid==0：将信号发送给与发送进程属于同一进程组的所有进程，且发送进程具有权限向这些进程发送信号
* pid<0：将信号发送给进程组ID等于pid绝对值，且发送进程具有权限向这些进程发送信号
* pid==-1：将信号发送给发送进程具有权限向它们发送信号的所有进程

signo编号0为空信号，kill仍进行正常的错误检查，但不发送信号，常用于确定一个特定进程是否仍然存在，若向一个并不存在的进程发送信号，则kill返回-1，errno被设置为ESRCH

##### 函数alarm和pause

alarm可以设置一个定时器，超时时产生SIGALRM信号，如果忽略或不捕捉此信号，默认动作是终止调用该函数的进程，大多数进程都会捕捉此信号

```
#include <unistd.h>

unsigned int alarm(unsigned int seconds);
// 返回值：0或者以前设置闹钟时间的余留秒数
```

* 每个进程只能有一个闹钟时间
* 如果调用alarm时，之前注册的闹钟时间没有超时，则余留值作为本次alarm调用的返回值，以前注册的闹钟时间被新值替换
* 如果调用alarm时，之前注册的闹钟时间没有超时，且本地alarm调用的参数是0，则取消以前的闹钟，余留值仍作为本次alarm调用的返回值

pause使调用进程挂起，直到捕捉一个信号

```
#include <unistd.h>

int pause(void);
// 返回值：-1， errno设置为EINTR
```

只有执行了一个信号处理函数并从其返回时，pause才返回

##### 信号集

```
#include <signal.h>

int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signo);
int sigdelset(sigset_t *set, int signo);
// 这四个函数，若成功，返回0，若出错，返回-1

int sigismember(sigset_t *set, int signo);
// 如果是返回1，如果不是，返回0，如果给定的信号无效，返回-1；
```

* sigemptyset：将信号集初始化为set指向的信号集，清除其中所有信号
* sigfillset：将信号集初始化为set指向的信号集中的信号，sigfillset或sigemptyset只执行一次
* sigaddset：把信号signo添加到信号集set中
* sigdelset：把信号signo从信号集set中删除
* sigismember：判断给定的信号signo是否是信号集中的一个成员

##### 函数sigprocmask

检测或更改进程的信号屏蔽字

```
#include <signal.h>

int sigpromask(int how, const sigset_t *restrict set, sigset_t *restrict oset);
// 若成功，返回0，若出错，返回-1
```

* 若oset是非空指针，那么进程的当前信号屏蔽字通过oset返回

* 若set是非空指针，那么参数how指示如何修改当前信号屏蔽字

  ```
  SIG_BLOCK：将set指向的包含了希望阻塞的信号集，与当前信号屏蔽字，相并，或操作
  SIG_UNBLOCK：将set指向的包含了希望阻塞的信号集的补集，与当前信号屏蔽字，相交，与操作
  SIG_SETMASK：将当前的信号集合设置为set指向的信号集，赋值操作
  ```

* 如果set是空指针，那么不改变进程的信号屏蔽字，how无意义

##### 函数sigpending

返回的信号集由参数set返回，对于调用进程而言，其中的各个信号是阻塞不能传递的，因而一定是当前未决的

```
#include <signal.h>

int sigpending(sigset_t *set);
// 若成功，返回0，若出错，返回-1
```

##### 函数sigaction

用于检查或修改与指定信号相关联的处理动作，取代了早期使用的signal函数

```
#include <signal.h>

int sigaction(int signo, conststruct sigaction*restrict act, struct sigaction*restrict oact);
// 若成功，返回0，若出错，返回-1

struct sigaction{
  void (*sa_handler)(int);
  sigset_t sa_mask;
  int sa_flag;
  void (*sa_sigaction)(int, siginfo_t*, void*);
};

sa_flag的选项：
SA_INTERRUPT: 由此信号中断的系统调用不自动重启动
SA_NOCLDSTOP: 若signo是SIGCHLD，当子进程停止，不产生此信号，当子进程终止，仍旧产生此信号，若已设置此标志，当停止的进程继续运行时，不产生SIGCHLD信号
SA_NOCLDWAIT:若signo是SIGCHLD，当调用进程的子进程终止时，不创建僵死进程，当调用进程随后调用wait，则阻塞到它所有子进程都终止
SA_NODEFER: 当捕捉到此信号执行其信号处理函数时，系统不自动阻塞此信号，应用于早期不可靠信号
SA_ONSTACK: XSI
SA_RESETHAND: ...
SA_RESTART: 由此信号中断的系统调用自动重启动
SA_SIGINFO: 对信号处理程序提供了附加信息：一个指向siginfo的指针以及指向上下文的context指针
```

* signo是要检测或修改的信号编号
* 若act非空，则修改其动作，若oact非空，则系统经由oact返回该信号的上一个动作
* sa_sigaction和sa_handler，在应用中只能一次使用其中之一

一般信号处理程序调用：

```
void handler(int signo);
```

如果设置sa_flag为SA_SIGINFO，则调用：

```
void handler(int signo, siginfo_t *info, void *context)
```

siginfo包含了信号产生原因有关信息：

```
typedef struct __siginfo {
	int	si_signo;		/* signal number */
	int	si_errno;		/* errno association */
	int	si_code;		/* signal code */
	pid_t	si_pid;			/* sending process */
	uid_t	si_uid;			/* sender's ruid */
	int	si_status;		/* exit value */
	void	*si_addr;		/* faulting instruction */
	union sigval si_value;		/* signal value */
	long	si_band;		/* band event for SIGPOLL */
	unsigned long	__pad[7];	/* Reserved for Future Use */
} siginfo_t;

union sigval {
	/* Members as suggested by Annex C of POSIX 1003.1b. */
	int	sival_int;
	void	*sival_ptr;
};
```

传递信号时，在si_value.sival_int传递一个整型或si_value.sival_ptr传递一个指针，SIGCHLD包含的si_code：

```
#define	CLD_EXITED	1	/* [XSI] child has exited */
#define	CLD_KILLED	2	/* [XSI] terminated abnormally, no core file */
#define	CLD_DUMPED	3	/* [XSI] terminated abnormally, core file */
#define	CLD_TRAPPED	4	/* [XSI] traced child has trapped */
#define	CLD_STOPPED	5	/* [XSI] child has stopped */
#define	CLD_CONTINUED	6	/* [XSI] stopped child has continued */
```

若信号是SIGCHLD，则设置si_pid, si_status和si_uid字段，若信号时SIGBUS、SIGILL、SIGFPE或SIGSEGV，则si_addr包含造成故障的根源地址，该地址可能并不准确

context是无类型参数，可被强制转为ucontext_t结构类型，用于标识信号传递时进程上下文

##### 函数sigsetjmp和siglongjmp

非局部转移的longjmp函数可以返回到程序的主循环，但是有一个问题，当捕捉到一个信号时，进入信号处理函数，此时当前信号被自动的加到进程的信号屏蔽字中，这就阻止了后来产生的这种信号中断该信号处理函数

```
#include <setjmp.h>

int sigsetjmp(sigjmp_buf env, int savemask);
// 若直接调用则返回0，若从siglongjmp调用返回则返回非0值

void siglongjmp(sigjmp_buf env, int val);
```

* 若savemask非0，则sigsetjmp在env中保存进程的当前信号屏蔽字
* 如果savemask非0，调用siglongjmp从其中恢复保存的信号屏蔽字

##### 函数sigsuspend

```
#include <signal.h>

int sigsuspend(const sigset_t *sigmask);
// 总是返回-1，且将errno设置为EINTR
```

* 进程的信号屏蔽字设置为sigmask
* 在捕捉到一个信号或发生了一个会终止该进程的信号之前，该进程被挂起
* 如果捕捉到一个信号且从该信号处理函数返回，则sigsuspend返回，且该进程的信号屏蔽字设置为调用sigsuspend之前的值

主要由三种用途：

* 保护代码临界区，使其不被特定信号中断
* 等待一个信号处理程序设置一个全局变量
* 实现父进程、子进程之间的同步

##### 函数abort

```
#include <stdlib.h>

void abort(void);
```

将SIGABRT信号发送给调用进程，进程不应忽略此信号

##### 函数system

POSIX要求system忽略SIGINT和SIGQUIT，阻塞SIGCHLD。Bourne shell的实现，终止状态是128+信号编号，如SIGINT是2，SIGQUIT是3

##### 函数sleep、nanosleep和clock_nanosleep

```
#include <unistd.h>

unsigned int sleep(unsigned int seconds);
// 返回0或未休眠的秒数
```

此函数使得调用进程挂起直到满足以下条件之一：

* 已经过了seconds的墙上时钟时间
* 调用进程捕获到一个信号并从信号处理程序返回

nanosleep提供纳秒级别的进度

```
#include <time.h>

int nanosleep(const struct timespec *reqtp, struct timespec *remtp);
// 若休眠到要求的时间，返回0，若出错，返回-1
```

* reqtp用秒和纳秒指定了需要休眠的时间长度
* 如果某个信号中断了休眠间隔，进程并没有终止，remtp就会被设置为未休眠完的时间长度，不需要就设置为NULL

随着多个系统时钟的引入，需要使用相对于特定时钟的延迟时间来挂起调用线程

```
#include <time.h>
int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *reqtp, struct timespec *remtp);
```

* clock_id指定了计算延迟基于的时钟
* flags为0表示相对时间，如休眠的时间长度，flags为TIMER_ABSTIME表示绝对时间，如某个特定时间，使用绝对时间可以改善进度

##### 函数sigqueue

使用排队必须做如下几个操作

1. 使用sigaction函数安装信号处理程序时指定SA_SIGINFO标志
2. 在sigaction结构的sa_sigacton而不是通常的sa_handler提供信号处理程序
3. 使用sigqueue发送信号

```
#include <signal.h>

int sigqueue(pid_t pid, int signo, const union sigval value);
// 若成功，返回0，若出错，返回-1
```

sigqueue只能把信号发送给单个进程，可以使用value传递整型或指针，发送的信号不能被无限排队，最大为SIGQUEUE_MAX

##### 信号名和编号

```
extern char *sys_siglist[];
```

某些系统提供这样的数组，下标是信号编号，数组元素是指向信号的信号名字符串的指针

```
#include <signal.h>

void psignal(int signo, const char *msg);
// 类似于perror
```

```
#include <signal.h>

void psiginfo(const siginfo *info, const char *msg);
// 工作方式与psignal类似，可以输出除了信号编号以外的更多信息
```

```
#include <string.h>

char *strsignal(int signo);
// 类似于stderror，返回描述该信号字符串的指针
```

