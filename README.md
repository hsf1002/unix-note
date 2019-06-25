# unix-note
《unix环境高级编程》笔记



### 第一章  UNIX基础知识

##### UNIX体系结构

* 内核

* 系统接口，内核的接口
* shell，一个特殊的应用程序，为其他应用程序提供了一个接口
* 公共函数库
* 应用程序

##### 登陆

登陆口令文件路径：

```
/etc/passwd
```

shell版本：

```
/bin/bash	Bourne-again shell
/bin/csh	C shell
/bin/ksh	Korn shell
/bin/sh		Bourne shell
/bin/tcsh	TENEX C shell
/bin/zsh
```

查看支持的：

```
cat /etc/shells
```

查看当前shell：

```
echo $SHEL
```

##### 文件和目录

* 文件结构：UNIX文件系统是目录和文件的一种层次结构，起点是根root目录，名称是一个字符/

* 文件名：只有斜线/和空格不能出现在文件名中，但是POSIX推荐只用字母、数字、下划线、句点、短横

* 绝对路径：以斜线/开头
* 相对路径：不以斜线/开头

##### 输入输出

* 文件描述符：通常是比较小的非负整数，内核用来标示一个特定进程正在访问的文件，打开或创建一个文件时返回一个fd，读或写时可以使用

* 每当运行一个程序，所有的shell都会默认打开三个文件描述符：标准输入、标准输出、标准错误，大多数shell都提供了任何一个或三个重定向到文件

  ```
  STDIN_FILENO
  STDOUT_FILENO
  ```

* 不带缓冲的IO：函数open、read、write、lseek、close提供了不带缓冲的IO

* 当程序将用户输入的内容复制到标准输出时，键入文件结束符，通常是Ctrl + D，终止本次复制

  ```
  ./a.out < infile > outfile 	// 将infile复制到outfile
  ```

* 标准IO：为不带缓冲的IO提供了带缓冲的接口，使用标准IO无需担心如何选取最佳缓冲区大小

##### 程序和进程

* 程序：一个存储在硬盘上的可执行文件，内核使用exec函数将程序读入内存执行
* 进程：程序的执行实例
* 进程ID：进程唯一的数字标识符，是一个非负整数
* 进程控制：fork-创建进程、exec-执行进程、waitpid-等待子进程执行完毕
* 线程：进程内的所有线程共享同一地址空间、文件描述符、栈等，但要保证共享数据的同步
* 线程ID：只在所属进程中起作用

##### 出错处理

UNIX系统函数出错，通常会返回一个负数，而且整型变量errno通常被设置为具有特定信息的值

* open出错会有15种不同的errno值
* errno.h中定义了errno以及可以赋予它的各种常量
* C标准定义了两个函数打印出错信息
  * strerror映射一个出错消息字符串，并返回这个字符串指针
  * perror在标准输出上产生一个出错消息，并返回

* 出错恢复：致命性的一般无法恢复，非致命性的典型恢复操作时延迟一段时间重试

##### 用户标识

* 用户ID：为0 的用户为root用户，超级管理员，在Mac下操作步骤：

  ```
  sudo su		// 输入当前用户密码
  sh-3.2#		// 连续输入两次root密码
  exit		// 退出当前shell
  su			// 输入刚才设置的root密码即可
  ```

* 组ID：`/etc/group`

* 附属组ID：允许一个用户属于最多16个其他的组

##### 信号

用于通知进程发生了某种情况，很多情况都会产生信号，键盘有两种产生信号的方式：

* 中断键：通常是Delete或者Ctrl + C，退出键：通常是Ctrl + \，它们用来中断当前进程
* 调用kill函数，限制是当向一个进程发信号时，我们必须是那个进程的所有者或者是超级用户

##### 时间值

UNIX使用两种不同的时间值

* 日历时间：UTC时间，1970年1月1日00:00:00开始累积的秒数，用time_t保存
* 进程时间：CPU时间，用来度量进程所使用的CPU资源，用时钟滴答来计算，用clock_t保存

##### 系统调用和库函数

UNIX为每个系统调用在标准C库中设置一个具有同样名字的函数，但是并非每个C函数都会使用系统调用，如printf会调用write这个系统调用，但是strcpy和atoi却不会使用任何系统调用。应用程序既可以调用系统调用，也可以使用相应的C库函数。系统调用通常提供一种最小接口，而库函数通常提供比较复杂的功能

### 第二章  UNIX标准及其实现

##### ISO C

International Organization for Standardization 国际标准化组织为了提高C语言的移植性而设立的C语言标准

```
assert.h	验证程序断言
complex.h	复数运算
ctype.h		字符分类与映射
errno.h		出错码
fenv.h		浮点环境
float.h		浮点常亮以及特性
inttypes.h	整形格式变换
iso646.h	赋值、关系以及一元操作宏
limits.h	编译时限制性常量
math.h		数学函数
setjmp.h	非局部跳转
signal.h	信号
stdarg.h	可变参数
stdbool.h	bool类型
stddef.h	标准定义
stdint.h	整形
stdio.h		标准输入输出
stdlib.h	标准函数
string.h	字符串相关函数
time.h		时间和日期
wctype.h	宽字符分类与映射
wchar.h		扩充的多字节和宽字符支持
```

##### IEEE POSIX

POSIX 是Portable Operating System Interfaces 的缩写，是由IEEE(Institute of Electrical and Electronics Engineers)制定的标准簇，包含了ISO C标准库函数  

POSIX标准定义的必须的头文件：

```
<dirent.h>	目录项	
<fcntl.h>	文件控制
<fnmatch.h>	文件名匹配类型	
<glob.h>	路径名模式匹配类型
<grp.h>		组文件	
<netdb.h>	网络数据库操作
<pwd.h>		口令文件	
<regex.h>	正则表达式
<tar.h>		tar归档值	
<termios.h>	终端I/O
<unistd.h>	符号常量	
<utime.h>	文件时间
<wordexp.h>	字扩展类型	
<arpa/inet.h>	Internet定义
<net/if..h>		套接字本地接口	
<netinet/in.h>	Internet地址族
<netinet/tcp.h>	传输控制协议定义	
<sys/mman.h>	内存管理声明
<sys/select.h>	select函数	
<sys/socket.h>	套接字接口
<sys/stat.h>	文件状态	
<sys/times.h>	进程时间
<sys/types.h>	基本系统数据类型	
<sys/un.h>		UNIX域套接字定义
<sys/utsname.h>	系统名	
<sys/wait.h>	进程控制
```

POSIX标准定义的XSI扩展头文件(26项)：

```
<cpio.h>	cpio归档值	
<dlfcn.h>	动态链接
<fmtmsg.h>	消息显示结构	
<ftw.h>		文件树漫游
<iconv.h>	代码集转换实用程序	
<langinfo.h>	语言信息常量
<libgen.h>		模式匹配函数定义	
<monetary.h>	货币类型
<ndbm.h>		数据库操作	
<nl_types.h>	消息类别
<poll.h>	轮询函数	
<search.h>	搜索表
<strings.h>	字符串操作	
<syslog.h>	系统出错日志记录
<ucontext.h>	用户上下文	
<ulimit.h>		用户限制
<utmpx.h>		用户帐户数据库	
<sys/ipc.h>		IPC
<sys/msg.h>		消息队列	
<sys/resource.h>	资源操作
<sys/sem.h>			信号量	
<sys/shm.h>			共享存储
<sys/statvfs.h>		文件系统信息	
<sys/time.h>		时间类型
<sys/timeb.h>		附加的日期和时间定义	
<sys/uio.h>			矢量I/O操作
```

POSIX标准定义的可选头文件(8项)：

```
<aio.h> 	异步I/O	
<mqueue.h>	消息队列
<pthread.h>	线程	
<sched.h>	执行调度
<semaphore.h>	信号量	
<spawn.h>	实时spawn接口
<stropts.h>	XSI STREAMS接口	
<trace.h>	时间跟踪
```

##### Single UNIX Specification

单一UNIX规范是POSIX标准的一个超集

##### UNIX系统实现

* SVR4：UNIX System V Release 4，AT&T的UNIX系统实验室的产品
* 4.4BSD：Berkeley Software Distribution，加州大学伯克利分校的计算机系统研究组研发
* FreeBSD：加州大学伯克利分校
* Linux：Linux Torvalds 在1991年为替代MINIX而研发，在GNU指导下是免费的
* Mac OS X：核心操作系统是Darwin，基于内核Mach、FreeBSD
* Solaris：Oracle开发的UNIX系统版本，基于SVR4

##### 限制

以下两种限制是必须的：

* 编译时限制：短整型的最大值是多少。可在头文件中定义
* 运行时限制：文件名长度的最大值是多少。要求进程调用一个函数获得限制值

提供了三种限制：

- 编译时限制定义在头文件中
- 与文件和目录无关的运行时限制，用sysconf函数判断
- 与文件和目录有关的运行时限制，用pathconf或fpathconf判断

*ISO C限制：*

所有编译时限制都定义在limit.h中

*POSIX限制：*

* 数值限制：LONG_BIT、SSIZE_MAX
* 最大值
* 最小值
* 运行时可以增加的值
* 运行时保持不变的值
* 其他不变值
* 路径名可变值

*XSI限制：*

* 最小值：NZERO（默认的进程优先级）、_XOPEN_NAME_MAX（文件名的字节数）等
* 运行时保持不变的值：IOV_MAX和PAGE_SIZE

**函数sysconf、pathconf和fpathconf**

调用这三个函数之一可以得到运行时限制

```
long sysconf(int name); 						// 适用于_SC开头的常量如 _SC_ARG_MAX
long pathconf(const char *pathname, int name); 	// 适用于_PC开头的常量如 _PC_PATH_MAX
long fpathname(int fd, int name); 				// 适用于_PC开头的常量如 _PC_PATH_MAX
```

* 如果name不是一个合适的常量，则返回-1

* 有些name会返回一个变量或者提示该值是不确定的，如果不确定则返回-1

* pathconf的pathname和fpathname的fd有很多限制，如果不满足，结果未定义

* _SC_CLK_TCK：每秒钟时钟滴答数，用于times函数的返回值

* _PC_MAX_CANON和 _PC_MAX_INPUT：引用的文件必须是终端文件

  ......

**不确定的运行时限制**

......

##### 选项

如同对限制的处理一样，POSIX定义了3种处理选项的方法：

* 编译时选项定义在unistd.h中
* 与文件和目录无关的运行时选项，用sysconf函数判断
* 与文件和目录有关的运行时选项，用pathconf或fpathconf判断

##### 基本系统数据类型

头文件sys/types.h定义的，还有些在其他头文件中，都是用typedef定义，大多数以_t结尾，用这种方式定义后，不用再考虑因系统不同而变化的程序实现细节

### 第三章 文件IO

##### 文件描述符

* 对内核而言，所有打开的文件都通过文件描述符引用
* 使用open或create返回的文件描述符标识该文件，将其作为参数传给read和write
* 非负整数，0-OPEN_MAX-1，STDIN_FILENO、STDOUT_FILENO、STDERR_FILENO可提高可读性

##### open和openat

```
#include <fcntl.h> 
 
int open(const char *path, int oflag,... /* mode_t mode */);   
int openat(int fd, const char *path, int oflag, ... /* mode_t mode */ );  
两函数的返回值：若成功，返回文件描述符；若出错，返回−1 
```

* O_RDONLY：只读打开
* O_WRONLY：只写打开
* O_RDWR：读写打开
* O_EXEC：只执行打开
* O_SEARCH：只搜索打开（目录）

mode必须指定至少一个，下面选项是可选

* O_APPEND：追加
* O_CREATE：不存在则创建
* O_DIRECTORY：如果path引用的不是目录，则出错
* O_EXCL：如果同时指定了O_CREATE，文件已经存在，则出错，可以测试一个文件是否存在，使得测试和创建成为一个原子操作

* O_SYNC：每次write等待物理I/O操作完成，数据和属性同步更新
* O_TRUNC：如果文件存在，且为只写/读写，打开成功，将其长度切断为0
* O_DSYNC：每次write等待物理I/O操作完成，如果写操作不影响，刚写入的数据，则不需等待文件属性被更新
* O_RSYNC：使得每个以文件描述符为参数进行的read操作等待，直至所有对文件同一部分挂起的写操作都完成

由open和openat函数返回的文件描述符一定是最小的未用的数值

* 常量_POSIX_NO_TRUNC决定是否要截断过长的文件名或路径名，还是返回一个错误

##### create

```
int create(const char *pathname, mode_t mode);	// 出错返回-1
```

等同于：

```
open（path, O_WRONLY|O_CREAT|O_TRUNC, mode);
```

##### close

关闭一个打开文件

```
int close(int fd);
```

当一个进程终止时，内核自动关闭它所有打开的文件

##### lseek

默认打开一个文件时，除非指定O_APPEND，否则偏移量为0

```
off_t lseek(int fd, off_t offset, int whence);	// 成功返回偏移量，出错返回-1
```

* SEEK_SET：距离文件开始处的offset个字节
* SEEK_CUR：距离当前偏移量的offset个字节
* SEEK_END：距离文件结尾处的offset个字节

---------------------------------------------------------------------

* 如果文件描述符指向一个管道、FIFO或网络套接字，返回-1，且errno为ESPIPE

* 测试lseek的返回值时需要谨慎，不要测试是否小于0，要测试是否等于-1
* lseek仅将文件偏移量记录到内核，不引起任何IO操作，该偏移量用于下一次读写
* 文件偏移量可以大于文件长度，对文件下一次的写操作将加长该文件并构成一个空洞（文件大小一样，但是占用的磁盘块不一样），没写过的字节被读为0

##### read

```
// 返回已经读到的字节数，若到文件尾，返回0，出错返回-1
ssize_t read(int fd, void * buf, size_t count);		
```

有多种情况可能导致实际读到的字节数小于要求读的字节数：

* 普通文件：读到要求的字节数前到达文件尾
* 从终端设备读取：通常一次只能读一行
* 从网络读取：网络的缓冲机制可能造成返回值小于要求读的
* 从管道或FIFO读取：若管道包含的字节小于要求读的
* 当一个信号造成中断，而已经读了部分数据量时

##### write

```
// 返回已经写入的字节数，出错返回-1
ssize_t write (int fd, const void * buf, size_t count);
```

##### IO的效率

大多数系统都会采用某种预读技术，当检测到正进行顺序读取时，系统就试图读入比应用所要求的更多数据，并假想应用很快就会读这些数据

##### 文件共享

当多个进程同时写一个文件时，则可能产生预想不到的结果，需要进行原子操作

##### 原子操作

* 追加一个文件：在打开文件时指定O_APPEND标志，就不需要在每次写之前lseek，内核已将其合并为一个原子操作
* pread和pwrite：pread相当于先lseek再read，虽然是个原子操作，但是无法中断其定位和读操作，也不会更新当前文件偏移量，pwrite功能和区别类似
* 创建一个文件：对 open 函数的 O_CREAT 和 O_EXCL 的同时使用，而该文件存在，open 将失败，否则创建该文件，并且使得文件是否存在的判定和创建过程成为原子操作

##### 函数dup和dup2

```
int dup(int fd);
int dup2(int fd, int fd2);
```

* dup返回的新文件描述符一定是当前可用文件描述符中的最小值
* dup2可以用fd2指定新的文件描述符的值，如果fd2已打开则关闭，如果fd2等于fd则返回fd2，而不关闭。否则fd2的FD_CLOEXEC文件描述符标志被清除，这样fd2在调用exec时是打开状态

```
dup(fd);
等同于：
fcntl(fd, F_DUPFD, 0);
```

```
dup2(fd, fd2);
等同于
close(fd);
fcntl(fd, F_DUPFD, fd2);
```

后一种情况，不完全等同

* dup2是原子操作，而close和fcntl之间可能被信号捕获函数打断而修改文件描述符
* dup2和fcntl有不同的errno

##### 函数sync、fsync和fddatasync

```
#include <unistd.h>
void sync(void)；
int fsync(int fd);		// 成功返回0，出错返回-1
int fdatasync(int fd);	// 成功返回0，出错返回-1
```

传统UNIX系统都在内核中设有缓冲区高速缓存或页高速缓存，大多数磁盘IO都通过缓冲区进行，这种方式称为延迟写，为了保持实际文件和缓冲区内容一致性

* sync：只是把修改过的块缓冲区排入写队列就返回，不等实际写操作结束，通常update的系统守护进程周期性的一般三十秒调用sync
* fsync：只对文件描述符fd指定的一个文件起作用，且等待写磁盘操作完成才返回
* fdatasync：类似于fsync，但只影响文件的数据部分，而fsync还会同步更新文件的属性

flush()仅仅刷新位于用户空间中的流缓冲区fflush()返回后，仅仅保证数据已不在流缓冲区中，并不保证它们一定被写到了磁盘。从流缓冲区刷新的数据可能已被写至磁盘，也可能还待在内核I/O缓冲区中，要确保流I/O写出的数据已写至磁盘，那么在调用fflush()后还应当调用fsync()

##### 函数fcntl

```
int fcntl(int fd, int cmd, ... /* arg */ );	// 成功返回依赖cmd，出错返回-1
```

可以实现以下5种功能：

* 复制一个已有的文件描述符（cmd=F_DUPFD或F_DUPFD_CLOEXEC）
* 获取/设置文件描述符标志（cmd=F_GETFD或F_SETFD）
* 获取/设置文件状态标志（cmd=F_GETFL或F_SETFL）
* 获取/设置异步IO所有权（cmd=F_GETOWN或F_SETOWN）
* 获取/设置记录锁（cmd=F_GETLK或F_SETLK）

cmd的取值有11种，如下：

- F_DUPFD：复制文件描述符，并返回新文件描述符，新的文件描述符的F_DUPFD_CLOEXEC被清除
- F_DUPFD_CLOEXEC：复制文件描述符，并返回新文件描述符，新文件描述符的F_DUPFD_CLOEXEC被设置
- F_GETFD：读取文件描述标识
- F_SETFD：设置文件描述标识
- F_GETFL：读取文件状态标识（O_RDONLY、O_WRONLY、O_RDWR、O_EXEC等）
- F_SETFL：设置文件状态标识，将文件状态标志设置为第三个参数
- F_GETOWN：获取当前接收SIGIO和SIGURG信号的进程ID或进程组ID
- F_SETOWN：设置接收SIGIO和SIGURG信号的进程ID或进程组ID（arg为正是指定一个进程ID，arg为负是指定绝对值等于arg的进程组ID）
- F_GETLK：如果已经被加锁，返回该锁的数据结构。如果没有被加锁，将l_type设置为F_UNLCK
- F_SETLK：给文件加上进程锁
- F_SETLKW：给文件加上进程锁，如果此文件之前已经被加了锁，则一直等待锁被释放

如果出错，所有命令都返回-1，如果成功则返回某个其他值，这四个命令有特定返回值：F_DUPFD返回新的文件描述符，F_GETFD和F_GETFL返回相应的标志，F_GETOWN返回一个正的进程ID或负的进程组ID

##### 函数ioctl

```
#include <sys/ioctl.h>
int ioctl(int fd, unsigned long request, ...); // 出错返回-1，成功返回其他值
```

IO操作的杂物箱

##### /dev/fd

较新的系统都会提供/dev/fd目录，名为0、1、2等的文件，打开/dev/fd/n等效于复制描述符n（假定n是打开的）

```
fd = open("/dev/fd/0", mode);
等同于：
fd = dup(0);
```

Linux对/dev/fd的实现是个例外，把文件描述符映射指向底层物理文件的符号连接，打开/dev/fd/0时，事实上正在打开与标准输入关联的文件，因此返回的新文件描述符的模式与/dev/fd文件描述符的模式并不相关

 ### 第四章  文件和目录

#####  函数stat、fstat、fstatat和lstat

```
#include <sys/stat.h>
int stat(const char *restrict pathname, struct stat * restrict buf);
int fstat(int fd, struct stat* buf);
int lstat(const char *restrict pathname, struct stat * restrict buf);
int fstatat(int fd, const char * restrict pathname, struct stat *restrict buf, int flag);
// 所有4个函数的返回值：若成功，返回0；若出错，返回-1.
```

* stat：返回与pathname相关的文件信息结构

  ```
  struct stat {
              dev_t st_dev;
              ino_t st_ino;
              mode_t st_mode;
              nlink_t st_nlink;
              uid_t st_uid;
              gid_t st_gid;
              dev_t st_rdev;
              off_t st_size;
              blksize_t st_blksize;
              blkcnt_t st_blocks;
              time_t st_mtime;
              time_t st_ctime;
              };
  ```

* fstat：获得在已在fd打开文件的有关信息

* lstat：返回符号链接而不是符号链接引用的文件信息

* fstatat：相对于当前打开目录的路径名返回文件统计信息

buf是一个指针，指向我们必须提供的结构

##### 文件类型

包括以下几种类型：

* 普通文件（regular file）：包含某种数据的文件。对于二进制的可执行文件而言，必须遵循一种标准化的格式，使内核能够确定程序文本和数据的加载位置
* 目录（directory file）
* 块特殊文件（block special file）
* 字符特殊文件（character special file）：系统中所有设备要么是块特殊文件，要么是字符特殊文件
* FIFO：命名管道，用于进程间通信
* 套接字（socket）：用于进程间的网络通信
* 符号链接（symbolic link）

可以通过宏来确定文件类型信息，参数是stat的st_mode中：

```
S_ISREG()	普通文件
S_ISDIR()	目录文件
S_ISCHR()	字符特殊文件
S_ISBLK()	块特殊文件
S_ISFIFO()	管道或FIFO
S_ISLNK()	符号链接
S_ISSOCK()	套接字

S_TYPEISMQ()	消息队列
S_TYPEISSEM()	信号量
S_TYPEISSHM()	共享存储对象
```

##### 设置用户ID和设置组ID

与一个进程相关的ID有6个或更多

```
我们实际上是谁：
实际用户ID
实际组ID

文件访问权限检查：
有效用户ID
有效组ID
附属组ID

由exec函数保存：
保存的设置用户ID
保存的设置组ID
```

通常，有效用户ID等于实际用户ID，有效组ID等于实际组ID

* 有效用户ID：stat中的st_uid
* 有效组ID：stat中的st_gid

可以在st_mode中设置标志（S_ISUID和S_ISGID），当执行一个程序时，将有效用户ID设置为文件所有者的用户ID st_uid，将有效组ID设置为文件组所有者ID st_gid

##### 文件访问权限

```
S_IRUSR：用户读权限
S_IWUSR：用户写权限
S_IXUSR：用户执行权限

S_IRGRP：用户组读权限
S_IWGRP：用户组写权限
S_IXGRP：用户组执行权限

S_IROTH：其他组都权限
S_IWOTH：其他组写权限
S_IXOTH：其他组执行权限
```

* 打开某个文件时，对该文件的每一级目录，包含隐藏的当前目录都必须有可执行权限
* 文件的读权限与open的O_RDONLY和O_RDWR标志有关
* 文件的写权限与open的O_WRONLY和O_RDWR标志有关
* 为了在open中指定O_TRUNC标志，必须具有写权限
* 为了在目录中创建新文件，必须对该目录具有写权限和执行权限
* 为了删除一个文件，必须对该目录具有写权限和执行权限，对该文件不需要有读写权限
* 如果用7个exec函数之一执行某个文件，必须具有执行权限，该文件必须是普通文件

##### 新文件和目录的所有权

open或create创建的新文件，用户ID是进程的有效用户ID，组ID可能实现为以下之一：

* 进程的有效组ID
* 所在目录的组ID

##### 函数access和faccessat

有时候希望按照实际用户ID和实际组ID来测试访问能力，测试可以调用这两个函数：

```
#include <unistd.h>
int access(const char *pathname, int mode);		// 成功返回0，出错返回-1
int faccessat(int fd, const char *pathname, int mode, int flag);	// 成功返回0，出错返回-1
```

* mode值可以为：F_OK（存在）   R_OK（具有读权限）   W_OK（具有写权限）   X_OK（具有执行权限）
* flag参数设置为AT_EACCESS，则访问检查的是进程的有效用户ID和有效组ID

##### 函数umask

```
#include <sys/stat.h>
mode_t umask(mode_t cmask);	// 返回值：之前的文件模式创建屏蔽字
```

```
read: 4
write: 2
execute: 1

umask值	 文件权限	  文件夹权限
--------------------------------
0			rw			rwx
1			rw			rw
2			r			rx
3			r			r
4			w			wx
5			w			w
6			x			x
7		no permission allowed
```

```
屏蔽位		含义
------------------------------
0400		用户读
0200		用户写
0100		用户执行
0040		组读
0020		组写
0010		组执行
0004		其他读
0002		其他写
0001		其他执行
```

* 创建一个新目录或文件时，默认权限通过umask查看
* 子进程的屏蔽字不会影响到父进程
* UNIX大多数用户不会处理umask的值，通常在登陆时由shell的启动文件设置一次
* 为了在进程中确保指定的访问权限已经激活，必须修改umask的值

##### 函数chmod、fchmod、fchmodat

这三个函数可以更改现有文件的访问权限

```
#include <sys/stat.h>
int chmod(const char *path, mode_t mode);
int fchmod(int fd, mode_t mode);
int fchmodat(int fd, const char *pathname, mode_t mode, int flag);
成功返回0，出错返回-1
```

* chmod在指定文件上操作
* fchmod则对已经打开的文件进行操作
* fchmodat：当pathname为绝对路径时，或者fd参数取值为AT_FDCWD而pathname为相对路径时，等同于chmod。否则，计算相对于由fd指向的打开目录的pathname，当flag设置为AT_SYMLINK_NOFOLLOW时，fchmodat不会跟随符号链接
* 为了改变一个文件的权限位，进程的有效用户ID必须等于文件的所有者ID，或者该进程必须具有超级用户权限

```
mode 			说明
--------------------------------
S_ISUID			执行时设置用户ID
S_ISGID			执行时设置组ID
S_ISVTX			保存正文（粘着位）

S_IRWXU			用户（所有者）读写执行
	S_IRUSR		用户（所有者）读
	S_IWUSR		用户（所有者）写
	S_IXUSR		用户（所有者）执行

S_IRWXG			组读写执行
	S_IRGRP		组读
	S_IWGRP		组写
	S_IXGRP		组执行

S_IRWXO			其他读写执行
	S_IROTH		其他读
	S_IWOTH		其他写
	S_IXOTH		其他执行
```

##### 粘着位

早期的系统当一个可执行程序文件的S_ISVTX被设置了，第一次启动被执行，结束后，程序正文部分的一个副本仍被保存，以便下次可以快速装入，现今系统大多都配置了虚拟存储系统以及快速文件系统，不再需要此项技术。现今已经扩展了粘着位的使用范围：如果对一个目录设置了粘着位，只有对目录具有写权限且满足下面条件之一，才能删除或重命名该目录下的文件：

* 拥有此文件
* 拥有此目录
* 时超级用户

目录/tmp和/var/tmp是设置粘着位 的典型代表，任何用户都可以在这两个目录创建文件，但是不能删除或重命名其他人的文件

##### 函数chown、fchown、fchownat和lchown

更改文件的用户ID和组ID，如果两个参数owner或group任意一个是-1，则对应的ID不变

```
int chown(const char *pathname,uid_t owner,gid_t group);
int fchown(int fd,uid_t owner,gid_t group);
int fchownat(int fd,const char *pathname,uid_t owner,gid_t group,int flag)
int lchown(const char *pathname, uid_t owner, gid_t group);     
// 4个函数的返回值：若成功，返回0；若出错，返回-1 
```

* 符号链接情况下（设置了AT_SYMLINK_NOFOLLOW），lchown和fchownat更改符号链接本身而不是指向文件的所有者

* fchown改变fd指向的打开文件的所有者，既然是已打开文件，不能作用于符号链接
* fchownat在两种情况下与chown或lchown相同：一是pathname是绝对路径，一是fd取值为AT_FDCWD而pathname为相对路径。在这两种情况下，如果flag设置了AT_SYMLINK_NOFOLLOW，fchownat和lchown行为相同，如果flag清除了此标志，fchownat与chown行为相同。如果fd为打开目录的文件描述符，且pathname是相对路径，fchownat函数计算相对于打开目录的pathname

##### 文件长度

* 普通文件：长度可以是0，从开始读到文件结束为止
* 目录：长度通常是一个数（16，512等）的整数倍
* 符号链接：长度是文件名的实际字节数，不包含结尾的null字节
* 现代系统大多提供了st_blksize：对文件I/O较合适的块长度 和 st_blocks：所分配的实际512字节块的个数
* 空洞：设置的偏移量超过文件尾端，并写入某些数据造成

```
ls -l core

du -s core		// 所使用的磁盘空间总量，多少个512字节（linux如果设置了环境变量POSIXLY_CORECT，是1024， 否则是512）

wc -c core		// 正常的IO操作读整个文件长度，-c计算字符数/字节数


如果用命令比如cat core > core.copy，则空洞会被实际数据0所填满
```

##### 文件截断

```
int truncate(const char *path, off_t length);	// 若成功，返回0；若出错，返回-1 
int ftruncate(int fd,off_t length);				// 若成功，返回0；若出错，返回-1 
```

* 如果文件本身长度大于length，则超过length以外的数据不能访问
* 如果文件本身长度小于length，文件长度将增加，以前的文件尾端和新的文件尾端之间的数据读作0，可能创造了一个空洞

##### 文件系统

可以把一个磁盘分成一个或多个分区，每个分区包含一个文件系统，i节点是固定长度的记录项，包含有关文件的大部分信息

* 硬链接：每个i节点都有一个链接计数，其值减少到0时，才可以删除该文件，这就是为什么解除一个文件的链接并不意味着释放该文件占用的磁盘块，也是为什么删除一个目录项的函数称为unlink而不是delete
* 符号链接：i节点的文件类型是S_IFLNK，实际内容是所指向文件的名字
* i节点包含了有关文件的所有信息：文件类型、文件访问权限、文件长度、指向文件数据块的指针等。stat中大多数信息来自i节点，只有文件名和i节点编号存放在目录项中
* 目录项中的i节点编号指向同一文件系统中的相应i节点，不能跨越文件系统，所以ln命令也不能跨越文件系统
* 不更换文件系统的情况下重命名一个文件，文件的实际内容并不移动，需要构造一个指向现有i节点的新目录项，并删除老的目录项，链接计数不变，这是mv命令的操作方式
* 任何一个页目录（不包含其他目录）的链接计数为2，即命名该目录的目录项和目录中的.项
* 非页目录的链接计数为3，即命名该目录的目录项、目录中的.项和子目录的..项

##### 函数link、linkat、unlink、unlinkat和remove

```
#include <unistd.h>
int link(const char* existingpath, const char* newpath);
int linkat(int efd, const char* existingpath, int nfd, const char* newpath，int flag);
// 成功返回0，失败返回-1
```

这两个函数创建一个新目录项newpath，引用现有文件existingpath，如果newpath存在，返回出错

* 任意路径名是绝对路径，则相应的文件描述符被忽略
* 现有文件是符号链接文件，flag参数设置了AT_SYMLINK_FOLLOW标志，就创建指向符号链接目标的链接，否则创建指向符号链接本身的链接
* 创建新目录项和增加链接计数是一个原子操作
* 如果实现支持创建指向一个目录的硬链接，也仅局限于超级用户可以这样做，理由是可能在文件系统中形成循环，大多数文件系统不能处理这种情况，很多系统实现不允许对于目录的硬链接

删除目录项，可以通过以下函数：

```
#include <unistd.h>
int unlink(const char* pathname);
int unlinkat (int fd,const char* pathname, int flag);
// 成功返回0，失败返回-1
```

* 如果pathname是符号链接，那么unlink删除该符号链接而不是所指向的文件
* 删除目录项并将pathname所引用文件的链接计数减1，如果该文件还有其他链接，仍然可以通过其他链接访问该文件的数据，如果出错，不对文件做任何修改
* 删除文件的条件：链接计数为0并且没有进程打开它
* 关闭文件的条件：打开它的进程数为0并且链接计数为0
* flat提供了一种方法改变unlinkat的默认行为，当AT_REMOVEDIR标志被设置，unlinkat可以类似于rmdir删除目录，如果这个标志被清除，则执行与unlink相同的操作
* unlink的这种特性可以被用来确保即使程序奔溃时，所创建的临时文件也不会遗留下来
* remove解除对一个文件或目录的链接。对于文件，remove与unlink相同，对于目录，remove与rmdir相同

##### 函数rename和renameat

可以对文件或目录进行重命名

```
#include <stdio.h>
int rename(const char *oldname, const char*newname);
int renameat(int oldfd, const char *oldname, int newfd, const char *newname);
// 成功返回0；失败返回-1
```

* 如果oldname是一个文件，为该文件或链接文件重命名。如果newname已存在，不能引用一个目录，如果newname已存在且不是一个目录，则先删除该目录项再将oldname重命名为newname，对于包含oldname和newname的目录，调用进程必须具有写权限
* 如果oldname是一个目录，为该目录重命名。如果newname已存在，则必须引用一个空目录（只有.和..），先将其删除，再将oldname重命名为newname。newname不能包含oldname作为其路径前缀
* 如果oldname或newname引用符号链接，则处理符号链接本身
* 不能对.和..重命名
* 如果oldname和newname引用同一个文件，不做任何修改返回

##### 符号链接

符号链接是对一个文件的间接指针，而硬链接直接指向文件的i节点，硬链接的限制：

* 通常要求硬链接和文件处于同一文件系统
* 只有超级用户能够创建指向目录的硬链接

符号链接指向何种对象没有文件系统限制，任何用户都可以创建指向目录的符号链接。是否跟随：链接到实际文件

* 跟随：access、chdir、chmod、chown、create、exec、link、open、opendir、pathconf、stat、truncate
* 不跟随：lchown、lstat、readlink、remove、rename、unlink

1. 符号链接的无限循环很容易消除，因为unlink并不跟随符号链接，而硬链接的这种循环很难消除
2. 当用open打开文件，如果参数路径是一个符号链接，则跟随指向所链接的文件
3. 符号链接可以指向不存在的文件，用 ls -l 可以查看，但是无法cat

##### 创建和读取符号链接

```
#include<unistd.h>
int symlink(const char*actualpath,const char *sympath);
int symlinkat(const char *actualpath,int fd,const char *sympath);
// 两个函数返回值：若成功，返回0;若出错，返回-1
```

* 创建符号链接，并不要求actualpath已经存在，actualpath和sympath也无需位于同一文件系统

* 因为open跟随符号链接，需要一种方式打开符号链接本身

```
ssize_t readlink(const char* restrict pathname,char *restrict buf,size_t bufsize);  
ssize_t readlinkat(int fd,const char* restrict pathname,char *restrict buf,size_t bufsize);
// 两个函数返回值：若成功，返回读取的字节数（不以null字节终止）；若出错，返回-1  
```

这两个函数组合了open、read和close的所有操作

##### 文件的时间

每个文件维护的三个字段

```
字段        说明                例子            ls选项
st_atim    文件数据最后访问时间    read          -u
st_mtim    文件数据最后修改时间    write         默认
st_ctim    i节点状态最后更改时间   chmod、chown  -c
```

* 影响i节点的操作：访问权限、更改用户ID、更改链接数等，但不修改文件内容，系统并不维护i节点的最后访问时间，access和stat函数并不更改其中的一个
* 创建一个文件影响包含该文件的目录，和i节点，读写操作只影响该文件的i节点，不影响目录

##### 函数futimens、utimensat和utimes

一个文件的访问时间和修改时间可以指定纳秒级别的时间戳进行修改

```
#include<sys/stat.h>
int futimens(int fd, const struct timespec times[2]);
int utimensat(int fd, const char* path, const struct time spec times[2], int flag);
// 两个函数返回值：若成功，返回0，如出错，返回-1
```

times数组第一个元素是访问时间，第二个是修改时间，都是日历时间（从1970/01/01 00:00:00开始的秒数）

* 如果times是空指针，则访问时间和修改时间设置为当前时间
* 如果times参数数组任意元素的tv_nsec为UTIME_NOW，相应的时间戳设置为当前时间，忽略tv_sec

```
struct timespec {
    time_t tv_sec; 	// seconds 
    long tv_nsec; 	// and nanoseconds 
};
```

* 如果times参数数组任意元素的tv_nsec为UTIME_OMIT，相应的时间戳保持不变，忽略tv_sec
* 如果times参数数组两个元素的tv_nsec既不是UTIME_NOW也不是UTIME_OMIT，相应的时间戳更改为tv_sec和tv_nsec字段的值

futimens、utimensat属于POSIX，而utimes在XSI扩展选项中

```
#include <sys/time.h>
int utimes(const char *filename, const struct timeval times[2]);
// 成功返回0，失败返回-1
```

```
struct timeval {
    long tv_sec;        /* seconds */
    long tv_usec;       /* microseconds */
};
```

注意：不能对状态更改时间st_ctim（i节点最近被修改的时间）指定一个值，因为调用utims时，此字段自动更新

##### 函数mkdir、mkdirat和rmdir

```
#include <sys/stat.h>     
int mkdir(const char *pathname, mode_t mode);     
int mkdirat(int fd, const char *pathname, mode_t mode);     
// 两个函数返回值：若成功，返回0；若出错，返回-1 
```

这两个函数创建一个新的空目录，其中.和..自动创建，所指定的文件访问权限mode由进程的文件模式屏蔽字修改

rmdir函数可以删除一个空目录

```
#include <unistd.h>     
int rmdir(const char *pathname);     
// 返回值：若成功，返回0；若出错，返回-1 
```

如果调用此函数使目录的链接计数成为0，并且也没有其他进程打开此目录，则释放由此目录占用的空间。如果在链接计数达到0时，有一个或多个进程打开此目录，则在此函数返回前删除最后一个链接及.和..项。另外，在此目录中不能再创建新文件。但是在最后一个进程关闭它之前并不释放此目录。（即使另一些进程打开该目录，也不能执行其他操作。为了使rmdir函数成功执行，该目录必须是空的）

##### 读目录

对某个目录具有访问权限的任意用户都可以读该目录，但只有内核才能写目录，一个目录的写/执行权限位决定了在该目录能否创建新文件以及删除文件，不代表能否写目录本身

```
#include<dirent.h>
DIR* opendir(constchar * path );
DIR* fdopendir(int fd);
// 这两个函数，若成功返回指针，若出错返回NULL

struct dirent *readdir(DIR *dp);	// 若成功返回指针，若出错或在目录尾返回NULL

void rewinddir(DIR *dp);
int closedir(DIR *dp);				// 若成功返回0，若出错返回-1

long telldir(DIR *dp);				// 返回值与dp关联的目录中的当前位置有关
void seekdir(DIR *dp,long loc);
```

DIR是一个结构，上述7个函数都用它保存当前正在被读的目录的有关信息，作用类似于FILE。由opendir和fdopendir返回的指针提供给另外5个函数使用

##### 函数chdir、fchdir和getcwd

每个进程都有一个当前工作目录，此目录是搜索所有相对路径名的起点。当前工作目录是进程的一个属性，起始目录则是登录名的一个属性。因为当前工作目录是进程的一个属性，所以它只影响调用chdir的进程本身，而不影响其他进程

```
#include <unistd.h>
int chdir( const char *pathname );
int fchdir( int filedes );
// 两个函数的返回值：若成功则返回0，若出错则返回-1
```

getcwd可以得到当前工作目录完整的绝对路径

```
#include <unistd.h>
char *getcwd( char *buf, szie_t size );
// 若成功则返回buf，若出错则返回NULL
```

当程序需要在文件系统中返回到其工作的起点时，getcwd函数是有用的。在更换工作目录之前，调用getcwd函数先将其保存起来。在完成了处理后，将从getcwd获得的路径名作为调用参数传送给chdir，就返回到了文件系统中的起点

##### 设备特殊文件

st_dev和st_rdev很容易引起混淆

* 每个文件系统所在的存储设备都由主次设备号表示，类型是dev_t，主设备号标识设备驱动程序，次设备号标识特定的子设备。同一磁盘的各个文件系统通常主设备号一样， 而次设备号不同
* 通常使用宏major和minor访问主次设备号
* 与每个文件名关联的st_dev的值是文件系统的设备号
* 只有字符特殊文件和块特殊文件才有st_rdev值，其包含实际设备的设备号

### 第五章 标准IO库

##### 流和FILE对象

对于标准IO库，操作时围绕流进行，freopen清除一个流的定向，fwide设置一个流的定向

```
#include <wchar.h>
int fwide(FILE *fp, int mode);
// 若流是宽定向返回正值，若流是字节定向返回负值，若流未定向，返回0
```

* 若mode为负，试图使指定的流字节定向
* 若mode为正，试图使指定的流宽定向
* 若mode为0，不设置定向，返回标识该流定向的值

fwide并不改变已定向流的定向

#### 缓冲

对于标准IO库，缓冲的目的是减少read和write的次数

* 全缓冲：填满IO缓冲区后才进行实际的IO操作

* 行缓冲：当输入或输出遇到换行时才进行实际的IO操作
* 不缓冲：不对字符进行缓冲存储

ISO C要求缓冲具有以下特征：

* 当且仅当标准输入输出不指向交互式设备，才是全缓冲
* 标准错误绝不会时全缓冲

但没说明标准输入输出指向交互式设备时，是行缓冲还是不缓冲，标准错误是行缓冲还是不缓冲，很多系统默认：

* 标准错误不带缓冲
* 若是指向终端设备的流，行缓冲，否则全缓冲

可以更改缓冲类型：

```
void setbuf(FILE *restrict fp, char *restrict buf);
void setvbuf(FILE *restrict fp, char *restrict buf, int mode, size_t size);
// 若成功返回0，若出错返回非0
```

* setbuf打开/关闭缓冲机制：打开就必须让buf指向长度为BUFSIZE的缓冲区，默认全缓冲，如果与终端设备相关，可能是行缓冲；关闭就将buf指向NULL
* setvbuf可以通过mode参数说明缓冲类型
  * _IOFBF：全缓冲
  * _IOLBF：行缓冲
  * _IONBF：不缓冲（忽略buf和size）

flush的两层含义：在标准IO库方面，是将缓冲区内容写到磁盘；在终端驱动方面，表示丢弃已存储在缓冲区的内容，任何时候都可以强制冲洗一个流

```
#include <stdio.h>
int fflush(FILE *fp);
// 若成功返回0，若出错，返回EOF
```

此函数将该流所有未写的数据传送到内核，如果fp为NULL，导致所有的流被冲洗

##### 打开流

```
FILE* fopen(const char* restrict pathname, const char* restrict type);  
FILE* freopen(const char* restrict pathname, const char* restrict type, FILE* restrict fp);  
FILE* fdopen(int filedes, char* type);  
// 三个函数，如果成功返回文件指针，失败则返回NULL
```

* fopen：打开一个指定的文件

* freopen：在一直指定的流上打开一个指定的文件，如若该流已经打开，则先关闭该流；如果该流已经定向，则先清除该定向。一般用于将一个指定的文件打开为一个预定的流：标准输入，标准输出或标准出错

* fdopen：获取一个现有的文件描述符，并使一个标准的IO流与该描述符相结合。常用于由创建管道和网络通信函数返回的描述符

type的类型：

二进制模式与文本模式操作相似，只不过是以二进制流的形式读写而已：

1. "r" 模式：O_RDONLY

   * 打开文件进行“**只读**”操作，即只能从文件读取内容

   * 若欲操作的**文件不存在，则打开失败**

   * 成功打开文件时，文件指针位于文件**开头**

   * 打开文件后，**不会清空**文件内原有内容

   * 可从文件中**任意位置**读取内容

2. "w" 模式：O_WRONLY|O_CREAT|O_TRUNC

   * 打开文件进行“**只写**”操作，即只能向文件写入内容

   * 若欲操作的**文件不存在，则新建文件**

   * 成功打开文件时，文件指针位于文件**开头**

   * 打开文件后，**会清空**文件内原有的内容

   * 可向文件中**任意位置**写入内容，且进行写入操作时，会**覆盖**原有位置的内容

3. "a" 模式：O_WRONLY|O_CREAT|O_APPEND

   * 打开文件进行“**追加**”操作，即只能向文件写入内容

   * 若欲操作的**文件不存在，则新建文件**

   * 成功打开文件时，文件指针位于文件**结尾**

   * 打开文件后，**不会清空**文件内原有内容

   * 只能向文件**末尾追加(写)**内容

4. "r+"模式：O_RDWR

   * 打开文件进行“**读写**”操作，即既可读取，又可写入

   * 若欲操作的**文件不存在，则打开失败**

   * 成功打开文件时，文件指针位于文件**开头**

   * 打开文件后，**不会清空文**件内原有内容

   * 无论是读取内容还是写入内容，都可在文件中**任意位置**进行，且进行写入操作时，会**覆盖**原有位置的内容

5. "w+"模式：O_RDWR|O_CREAT|O_TRUNC

   * 打开文件进行“**读写**”操作，即既可读取，又可写入

   * 若欲操作的**文件不存在，则新建文件**

   * 成功打开文件时，文件指针位于文件**开头**

   * 打开文件后，**会清空**文件内原有的内容

   * 无论是读取内容还是写入内容，都可在文件中**任意位置**进行，且进行写入操作时，会**覆盖**原有位置的内容

6. "a+"模式：O_RDWR|O_CREAT|O_APPEND

   * 打开文件进行“**读写**”操作，即既可读取，又可写入

   * 若欲操作的**文件不存在，则新建文件**

   * 成功打开文件时，文件指针位于文件**结尾**

   * 打开文件后，**不会清空**文件内原有内容

   * 读取内容时，可以在**任意位置**进行，但写入内容时，只会**追加**在文件尾部

```
限制              R   w   a   r+   w+   a+
文件必须存在       Y            Y
放弃文件之前内容    Y            Y

流可读            Y            Y    Y     Y
流可写                Y    Y   Y    Y     Y
流只可在尾端写              Y              Y
```

调用flose可以关闭一个打开的流

##### 读写流

**每次一个字符的IO：**

输入函数：以下三个函数一次读一个字符

```
int getc (FILE * fp);
int fgetc (FILE * fp);    // f代表的是function
int getchar(void);
// 三个函数若成功返回下一个字符，若出错或到达文件尾，返回EOF
```

为了区分出错还是文件尾，可以调用：

```
int ferror(FILE * fp);
int feof(FILE *fp);
// 两个函数若条件为真，返回非0，否则，返回0
void clearerr(FILE *fp)
// 可以清除出错标志和文件结束标志
```

从流中读出的数据，可以调用ungetc将字符压送回流中，压送后可以再次读出，读出字符的顺序与压送顺序相反：

```
int ungetc(int c, FILE *fp);
// 若成功，返回c，若出错，返回EOF
```

* 一次只能回送一个字符
* 回送的字符，不一定是上一次读到的字符
* 正在读一个输入流，并进行某种形式的切词或记号切分操作，会经常用到回送字符操作

输出函数：与三个输入函数对应

```
int putc (int c, FILE * fp);
int fputc (int c, FILE * fp);
int putchar(int c);
// 三个函数若成功返回c，若出错，返回EOF
```

* getchar等同于getc(stdin)，putchar等同于putc(c, stdout)

* getc和putc可以实现为宏，而fgetc和fputc是函数

**每次一行的IO：** 

```
char *fgets(char *restrict buf, int n, FILE *restrict fp);
char *gets(char *buf);
// 这两个函数若成功返回buf，若出错或到达文件尾，返回NULL
```

* gets从标准输入读，fgets从指定的流读

* gets不被推荐，ISO C最新版本以及忽略，原因是不能指定缓冲区大小可能造成缓冲区溢出

```
int fputs(const char *restrict str, FILE *restrict fp);
int puts(const char *str);
// 这两个函数若成功返回非负值，若出错，返回EOF
```

* puts虽然不像gets那样不安全也要避免使用，以免需要记住它的最后是否加了一个换行符
* 总是用fgets和fputs，每行终止都必须自己处理换行符

##### 标准IO的效率

```
fgets/fputs   best?
getc/putc
fgetc/fputc
```

exit函数会冲洗任何未写的数据，然后关闭所有打开的流，使用标准IO的优点是无需考虑缓冲及最佳IO长度的选择

##### 二进制IO

对于二进制文件，如果一次读一个字符，麻烦且费时，如果一次读一行，fputs在遇到null字节会停止，且结构中可能含有null字节，类似如果输入数据中包含null字节或换行符，fgets无法正常工作

```
size_t fread(void *restrict ptr, size_t size, size_t count, FILE *restrict fp);
size_t fwrite(const void *restrict buf, size_t size, size_t count, FILE *restrict fp);
// 两个函数的返回值：读或写的对象数
// size是每个数组或结构体的长度，count是读写的元素个数
```

使用这两个函数可以轻易的读写一个二进制数组或一个结构

```
float dat[10];

if (fwrite(&data[2]), sizeof(float), 4, fp) != 4)
   err
   
struct{
    short count;
    long total;
    ...
}

if (fwrite(&item, sizeof(item), 1, fp) != 1)
	err
```

返回值可能小于所要求的count：读如果出错或到达文件尾，调用ferror或feof判断是哪种情况，写如果小于则出错

##### 定位流

三种方法：

* ftell和fseek：假定文件的位置可以存放在一个长整型中

```
long ftell(FILE *fp);	// 若成功，返回文件位置，若出错，返回-1L
int fseek(FILE *fp, long offset, int fromwhere);	// 若成功，返回0，若出错，返回-1
void rewind(FILE *fp);  // 可以将一个流设置到起始位置
```

* ftello和fseeko：使用off_t代替了长整型

```
long ftello(FILE *fp);	// 若成功，返回文件位置，若出错，返回off_t-1
int fseeko(FILE *fp, off_t offset, int fromwhere);  // 若成功，返回0，若出错，返回-1
```

* fgetpos和fsetpos：使用一个抽象数据类型fpos_t记录文件位置（需要移植到非UNIX系统的程序应该使用）

```
int fgetpos(FILE *restrict fp, fpos_t *restrict pos);
int fsetpos(FILE *fp, const fpos_t *pos);
// 两个函数若成功，返回0，若出错，返回非0
```

##### 格式化IO

```
int printf(const char *restrict format, ...);
int fprintf(FILE *restrict fp, const char *restrict format, ...);
int dprintf(int fd, const char *restrict format, ...);
// 三个函数，若成功，返回输出字符数，若出错，返回负值
int sprintf(char *restrict buf, const char *restrict format, ...);
// 若成功，返回存入数组的字符数，若出错，返回负值
int snprintf(char *restrict buf, size_t size, const char *restrict format, ...);
// 如缓冲区足够大，返回将要存入的字符数，若编码出错，返回负值
```

* printf：将格式化数据写到标准输出
* fprintf：写到指定的流
* dprintf：写到指定的文件描述符
* sprintf：送入数组buf，尾端自动加null字节，但该字节不包含在返回值中，缓冲区可能溢出
* snprintf：为了防止缓冲区溢出，指定了缓冲区长度

参数长度说明：

```
hh  signed/unsigned char
h   signed/unsigned short
l   signed/unsigned long
ll  signed/unsigned long long
j   intmax_t/unintmax_t
z   size_t
t   ptrdiff_t
L   long double
```

如何解释参数：

```
d/i  有符号十进制
o    无符号八进制
u    无符号十进制
x/X  无符号十六进制
c    字符，若lc则宽字符
s    字符串，若ls则宽字符串
p    void指针
%    %
C    等效lc
S    等效ls
```

格式化输入：

```
int scanf(const char *restrict format, ...);
int fscanf(FILE *restrict fp, const char *restrict format, ...);
int sscanf(const char *restrict buf, const char *restrict format, ...);
// 三个函数，若成功返回赋值的输入项数，若出错或到达文件尾，返回EOF
```

##### 实现细节

可以对一个流调用fileno获取其描述符，如果要调用dup或fcntl等函数，则需要次函数

```
int fileno(FILE *fp);
// 返回与流关联的文件描述符
```

##### 临时文件

```
#include <stdio.h>
char *tmpnam(char *ptr);  // 返回指向唯一路径名的指针
FILE *tmpfile(void);      // 若成功，返回文件指针，若出错，返回NULL
```

* tmpnam产生一个与现有文件名不同的一个有效路径名字符串
* tmpfile创建一个临时二进制文件（wb+），关闭该文件或程序结束将自动删除这种文件

一般先调用tmpnam先生成一个唯一的路径名，再用这个路径名tmpfile创建一个临时文件，并立即unlink它，解除链接并不删除其内容，关闭该文件才会删除其内容

```
#include <stdlib.h>
char *mkdtemp(char *template);  // 若成功，返指向目录的指针，若出错，返回NULL
int *mkstemp(char *template);   // 若成功，返回文件描述符，若出错，返回-1
```

template这个字符串最后6位设置为XXXXXX的路径名

* mkdtemp创建一个具有唯一名字的目录，mkstemp创建一个具有唯一名字的文件

* 与tmpfile不同，mkstemp创建的临时文件不会自动删除，必须手动解除链接
* 使用tmpnam和tmpfile有个缺点，在返回唯一的路径名和使用该路径名之间有一个时间窗口，可能被其他进程先创建，而使用mkstemp不存在这个问题

##### 内存流

三个函数可以用于内存流的创建：

* fmemopen：
  * 无论何时以追加方式打开，当前文件位置设置为缓冲区第一个null字节，如果缓冲区不存在null字节，则当前位置设置为缓冲区结尾的后一个字节；如果不是追加方式，当前位置设置为缓冲区的开始位置
  * 如果buf是null，打开流进行读写没有意义
  * 任何时候需要增加缓冲区中数据量以及调用fclose、fflush、fseek、fseeko、fsetpos时都会在当前位置写入一个null字节

```
FILE *fmemopen(void *restrict buf, size_t size, const char *restrict mode);
// 若成功，返回流指针，若出错，返回NULL
```

buf指向缓冲区，size指明缓冲区大小，mode控制如何使用流

* open_memstream：

```
FILE* open_memstream(char **ptr, size_t* sizeloc);
// 若成功，返回流指针，若出错，返回NULL
```

* open_wmemstream：

```
#include <wchar.h>
FILE* open_wmemstream(wchar_t **buf, size_t *size);
// 若成功，返回流指针，若出错，返回NULL
```

这两个与fmemopen不同之处：

1. 创建的流只能写打开
2. 不能指定缓冲区，可以分别通过buf和size参数访问缓冲区地址和大小
3. 关闭流需要自行释放缓冲区
4. 对流添加字节会增加缓冲区大小

缓冲区大小和地址有一些原则：

1. 缓冲区地址和大小只有在调用fclose或fflush后才有效
2. 这些值只有在下一次流写入或调用flclose前才有效

### 第六章  系统数据文件和信息

##### 口令文件

/etc/passwd  

用户名：加密口令：用户ID：组ID：注释字段：初始工作目录：初始shell

* 通常有一个用户名为root，用户ID为0的登录项（超级用户）
* 加密口令字段占位，内容存储在了另一个文件
* 某些字段可能为空，如果加密口令字段为空，意味着该用户没有口令
* shell字段包含一个可执行程序，用作登陆shell；为空，则取系统默认，如果是/dev/null，标明是一个设备，不是可执行文件，任何人无法以该用户登陆
* 为了阻止一个特定用户登陆，除了/dev/null外，还可以将/bin/false作为登陆shell，或/bin/true
* 使用nobody用户名的目的使任何人都可以登陆，只能访问人人皆可读写的文件
* 使用finger指令支持注释字段的附加信息

```
#include <pwd.h>
struct passwd *getpwnam(const char *name);
struct passwd *getpwuid(uid_t uid);
// 两个函数，若成功，返回指针，若出错，返回NULL

struct passwd {
                char *pw_name; /* user name */
                char *pw_passwd; /* user password */
                uid_t pw_uid; /* user id */
                gid_t pw_gid; /* group id */
                char *pw_gecos; /* real name */
                char *pw_dir; /* home directory */
                char *pw_shell; /* shell program */
              };
```

getpwuid由ls命令使用，getpwnam由login程序使用，只能查看登录名或用户ID，如果查看口令文件，需要：

```
#include <pwd.h>
struct passwd *getpwent(void);
// 若成功，返回指针，若出错或到达文件尾，返回NULL

void setpwent(void);
void endpwent(void);
```

* getpwent：返回口令文件中下一个记录项

* setpwent：将getpwent的读写地址指向密码文件开头
* endpwent：关闭这些文件

##### 阴影口令

/etc/shadow

加密口令是经单向加密算法处理过的用户口令副本，通常将加密口令存放在一个阴影口令的文件中，仅有几个用户ID为root的程序如login和passwd才可以访问

```
struct spwd 
{
    char *sp_namp; /* Login name */
    char *sp_pwdp; /* Encrypted password */
    long int sp_lstchg; /* Date of last change */
    long int sp_min; /* Minimum number of days between changes */
    long int sp_max; /* Maximum number of days between changes */
    long int sp_warn; /* Number of days to warn user to change the password */
    long int sp_inact; /* Number of days the account may be inactive */
    long int sp_expire; /* Number of days since 1970-01-01 until account expires */
    unsigned long int sp_flag; /* Reserved */
};
```

这组函数与口令文件的一组函数对应：

```
#include <shadow.h>
struct spwd *getspnam(const char *name);
struct spwd getspent(void);
// 两个函数，若成功，返回指针，若出错或到达文件尾，返回NULL

void setspent(void);
void endspent(void);
```

##### 组文件

/etc/group

查看组名或组ID：

```
#include<grp.h>
struct group *getgrpid(gid_t gid);
struct group *getgrnam(const char *name);
// 两个函数，若成功，返回指针，若出错，返回NULL
```

如果要搜索整个组文件：

```
#include<grp.h>
struct group *getgrent();
// 若成功，返回指针，若出错或到达文件尾，返回NULL
void setgrent();
void endgrent();
```

##### 附属组

使用附属组可以不必再显式的经常更改组，文件访问权限不仅检查组ID，还要检查附属组ID，一个用户可能参与多个项目，因此需要同时属于多个组

```
#include<unistd.h>
int getgroups(int gidsetsize, gid_t grouplist[]);  
// 若成功，返回附属组ID数，若出错，返回-1，将附加组id填写到grouplist中，最多gidsetsize个

#include<grp.h>
#include<unistd.h>/*非linux*/
int setgroups(int ngroups, const gid_t grouplist[]);   // 为进程设置附加组id表

#include<grp.h>   /*on linux and Solaris*/
#include<unistd.h>/*非linux 和 solaris*/
int initgroups(const char* username, gid_t basegid);
// 两个函数，若成功，返回0，若出错，返回-1
```

* getgroups：将进程所属用户的各附属组ID填写到数组中，如gidesetsize为0，返回附属组ID数
* setgroups：由超级用户调用为调用进程设置附属组ID表
* initgroups：只有超级用户才会调用initgroups，只有initgroups才调用setgroups

##### 其他数据文件

```
口令 /etc/passwd  <pwd.h>  passwd  getpwnam,getpwuid
组   /etc/group  <grp.h>  group  getgrnam,getgrgid
阴影 /etc/shadow  <shadow.h> spwd  getspnam
主机 /etc/hosts  <netdb.h> hostent  gethostbyname,gethostbyaddr
网络 /etc/network  <netdb.h> netend  getnetbyname,getnetbyaddr
协议 /etc/protocols  <netdb.h> protoent getprotobyname,getprotobynumber
服务 /etc/services  <netdb.h> servent  getservbyname,getservbyport
```

对于每个文件，至少有三个函数：

* get：读取下一条记录，如果需要，还会打开数据文件
* set：打开数据文件
* end：关闭数据文件

##### 登陆账户记录

utmp（who读取）文件记录当前登陆到系统的各个用户，wtmp（last读取）跟踪各个登陆和注销事件

##### 系统标识

uname：返回主机和操作系统相关信息

```
#include<sys/utsname.h>  
int uname(struct utsname *name);
// 若成功，返回非负值，若出错，返回-1
```

gethostname：只返回主机名，通常是TCP/IP网络上主机名

```
#include<unistd.h>        
int gethostname(char *name, int namelen);
// 若成功，返回0，若出错，返回-1
```

##### 时间和日期

**time：返回当前时间和日期，即日历时间**

```
include<time.h>       
time_t time(time_t *calptr);
// 若成功，返回时间值，若出错，返回-1

typedef long     time_t;    /* 时间值time_t 为长整型的别名*/
```

如果参数为非空，时间值也存放在calptr中

**clock_gettime：获取指定时钟的时间**

```
int clock_gettime(clockid_t clk_id, struct timespec *tp);
// 若成功，返回0，若出错，返回-1
int clock_settime(clockid_t clk_id, const struct timespec *tp);
// 若成功，返回0，若出错，返回-1

struct timespec {
    time_t tv_sec; 	// seconds 
    long tv_nsec; 	// and nanoseconds 
};
```

* CLOCK_REALTIME：系统实时时间，随系统实时时间改变而改变
* CLOCK_MONOTONIC：从系统启动这一刻起开始计时，不受系统时间被用户改变
* CLOCK_PROCESS_CPUTIME_ID：本进程到当前代码系统CPU花费的时间
* CLOCK_THREAD_CPUTIME_ID：本线程到当前代码系统CPU花费的时

**gettimeofday：已经弃用，有些程序还在使用，提供了比time更高的精度（微秒）**

```
#include<time.h>    
int gettimeofday(struct timeval *restrict tp, void *restrict tzp);
// 总是返回0，tzp的唯一合法值是NULL

struct timeval {
    long tv_sec;        /* seconds */
    long tv_usec;       /* microseconds */
};
```

##### gmtime和localtime：将日历时间转为分解的时间

秒可以超过59的理由是可以表示润秒

```
#include<time.h>
struct tm *gmtime(const time_t *calptr);  		
// 转化为国际标准时间的年、月、日、时、分、秒等，若出错，返回NULL
struct tm *localtime(const time_t *calptr);     
// 转化为本地时间(考虑到本地时区和夏令时标志)，若出错，返回NULL

struct tm {
   int tm_sec;    /* Seconds (0-60) */
   int tm_min;    /* Minutes (0-59) */
   int tm_hour;   /* Hours (0-23) */
   int tm_mday;   /* Day of the month (1-31) */
   int tm_mon;    /* Month (0-11) */
   int tm_year;   /* Year since 1900 */
   int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
   int tm_yday;   /* Day in the year (0-365, 1 Jan = 0) */
   int tm_isdst;  /* Daylight saving time */
};
```

##### mktime：本地时间转为time_t

```
#include<time.h>  
time_t mktime(struct tm *tmptr);
// 若成功，返回日历时间，若出错，返回-1
```

##### strftime：类似于printf的时间值函数，通过可用的多个参数定制产生的字符串

```
#include<time.h>     
size_t strftime(char *restrict buf, size_t maxsize, const char *restrict format, const struct tm *restrict tmptr);
size_t strftime(char *restrict buf, size_t maxsize, const char *restrict format, const struct tm *restrict tmptr, locale_t locale);
// 两个函数，若有空间，返回存入数组的字符数，否则，返回0

%a	Abbreviated weekday name *	Thu
%A	Full weekday name *	Thursday
%b	Abbreviated month name *	Aug
%B	Full month name *	August
%c	Date and time representation *	Thu Aug 23 14:55:02 2001
%C	Year divided by 100 and truncated to integer (00-99)	20
%d	Day of the month, zero-padded (01-31)	23
%D	Short MM/DD/YY date, equivalent to %m/%d/%y	08/23/01
%e	Day of the month, space-padded ( 1-31)	23
%F	Short YYYY-MM-DD date, equivalent to %Y-%m-%d	2001-08-23
%g	Week-based year, last two digits (00-99)	01
%G	Week-based year	2001
%h	Abbreviated month name * (same as %b)	Aug
%H	Hour in 24h format (00-23)	14
%I	Hour in 12h format (01-12)	02
%j	Day of the year (001-366)	235
%m	Month as a decimal number (01-12)	08
%M	Minute (00-59)	55
%n	New-line character ('\n')	
%p	AM or PM designation	PM
%r	12-hour clock time *	02:55:02 pm
%R	24-hour HH:MM time, equivalent to %H:%M	14:55
%S	Second (00-61)	02
%t	Horizontal-tab character ('\t')	
%T	ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S	14:55:02
%u	ISO 8601 weekday as number with Monday as 1 (1-7)	4
%U	Week number with the first Sunday as the first day of week one (00-53)	33
%V	ISO 8601 week number (00-53)	34
%w	Weekday as a decimal number with Sunday as 0 (0-6)	4
%W	Week number with the first Monday as the first day of week one (00-53)	34
%x	Date representation *	08/23/01
%X	Time representation *	14:55:02
%y	Year, last two digits (00-99)	01
%Y	Year	2001
%z	ISO 8601 offset from UTC in timezone (1 minute=1, 1 hour=100)
If timezone cannot be termined, no characters	+100
%Z	Timezone name or abbreviation *
If timezone cannot be termined, no characters	CDT
%%	A % sign	%
```

##### strptime：是strftime反过来的版本

```
字符串                                   格式化字符串
    \                                       /
  strptime                             strftime
          \                             /
                struct_tm(分解时间)
                 gmtime |localtime
                        |
                        |mktime
          tv_sec                       tv_sec
timeval------------>time_t(日历时间)<------------timespec
   \                    |                       /                    
      \                 |                   /                     
          \             | time         /
 gettimeofday \         |          / clock_time                        
                       内核
```

localtime、mktime和strftime受到环境变量TZ时区的影响，如果定义了TZ则替换系统默认，如果TZ为空，则使用UTC

### 第七章  进程环境

##### main 函数

内核执行C程序调用main函数之前会先调用一个特殊的启动例程，可执行程序将此启动例程指定为程序的起始地址，这由连接器设置，而连接器由编译器调用

##### 进程终止

有八种方式，前五种正常终止，后三种异常终止

* 从main返回
* 调用exit
* 调用_exit 或 _Exit
* 最后一个线程从启动例程返回
* 最后一个线程调用pthread_exit
* 调用abort
* 接收一个信号
* 最后一个线程对取消请求作出响应

**退出函数**

```
#include <stdlib.h>
void exit(int staus);
void _Exit(int staus);

#include <unistd.h>
void _exit(int status);
```

* _exit 和 _Exit立刻进入内核
* exit先执行一些清理工作（对所有打开的流调用flose）然后返回内核
* 终止状态：
  * 如果不带终止状态或main执行了一个无返回值的return或main没有声明返回类型为整型，则程序终止状态未定义
  * 如果main返回类型是整型，且执行到最后一句返回，终止状态是0

* 内核调用程序的唯一方法是exec函数，进程自愿终止的唯一方法是显式或隐式（exit）的调用_exit 或 _Exit

**函数atexit**

终止处理函数：一个进程最多可以登记32个函数，由exit自动调用

```
#include <stdlib.h>
int atexit (void (*func)(void));
// 若成功，返回0，若出错，返回非0
```

无参数，也无返回值的登记函数，exit调用它们的顺序与它们登记的顺序相反，同一函数登记多次，也被调用多次

##### 命令行参数

ISO C和POSIX 1都要求argv[argc]是一个空指针

##### 环境表

每个程序都有一张环境表，与参数表一样，是一个指针数组，每个指针（环境指针）指向一个环境变量字符串，而全局变量environ包含了该指针数组的地址。大多数系统支持main带三个参数，其中第三个就是环境表地址，而ISO C和POSIX 1都规定使用environ而不是第三个参数，通常用getenv和putenv来访问特定的环境变量，要查看整个环境，必须使用environ指针

##### C程序的存储空间

* 正文段：CPU执行的机器指令，通常可共享
* 初始化数据段：明确赋值的变量
* 未初始化数据段：bss段（block started by symbol）
* 栈：自动变量以及函数调用保存的信息
* 堆：动态分配内存

##### 共享库

使得可执行程序不再需要包含公用的库函数，只需在所有进程可引用的存储区保存这个库的一个副本。程序第一次执行或者第一次调用某个库函数时，用动态链接将程序与共享库函数链接，减少了每个可执行程序的长度，但增加了一些运行时开销

```
gcc 13.c 		// gcc默认使用共享库
-rwxr-xr-x  1 sky  staff  8616  6  7 07:21 a.out*

gcc -static 13.c 		// gcc阻止使用共享库，macOS报错
ld: library not found for -lcrt0.o
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```

##### 存储空间分配

* malloc：指定字节数，初始值不确定
* calloc：为指定数量指定长度的对象分配空间，初始值为0
* realloc：重新分配空间，可能需要将当前分配区的内容移到另一个足够大的区域，新增区的初始值不确定
* alloca：在当前函数的栈帧上分配空间，无需考虑释放内存，但是某些系统在函数调用后不允许增加栈帧长度，linux是允许的

```
#include <stdlib.h>
void* malloc(size_t size);
void* calloc(size_t nobj, size_t size); 
void* realloc(void* ptr, size_t newsize);  
// 若成功，返回非空指针，若出错，返回NULL
```

这三个函数返回的指针一定是适当对齐的，使其可用于任何数据对象。多数实现所分配的空间比实际要稍大，用来存储分配块的长度、指向下一个分配块的指针等，如果超过一个分配区的尾端或起始位置之前进行写操作，则会改变另一个块的管理记录信息，这种错误是灾难性，不会马上暴露，很难发现

##### 环境变量

应该使用getenv获取一个环境变量的值，而不是直接访问environ

```
#include<stdlib.h>
char *getenv(const char *name);
// 若成功，返回一个以 null 结尾的字符串，为被请求环境变量的值，若出错，返回 NULL
```

设置环境变量只能影响当前进程和其后生成和调用的任何子进程的环境，不能影响父进程的环境

```
int putenv(const char* str);  // 若成功，返回0，若错误，返回-1
int setenv(const char* name, const char* value, int overwrite) // 若成功，返回0，若错误，返回-1
int unsetenv(const char* name)  // 若成功，返回0，若错误，返回-1 
```

* putenv：取形式为name=value的字符串，将其放到环境表中，如name已存在，先删除原定义
* setenv：将name设置为value，如name已存在，那么a-若rewrite非0，先删除原定义；b-若rewrite为0，则不删除（name不设置为新的value，也不出错）。该函数必须分配存储空间
* unsetenv：删除name的定义，即使不存在也不算出错

##### setjmp和longjmp

goto语句无法跨越函数，执行这类跳转功能的函数式setjmp和longjmp。如果函数层次比较深，以检查返回值的方法逐层返回，就会很麻烦，而setjmp和longjmp可以跳过若干调用栈帧，返回到当前函数调用路径的某一函数

```
#include <setjmp.h>
int setjmp(jmp_buf env);	// 若直接调用，返回0，若从longjmp返回，则非0
void longjmp(jmp_buf env, int value); // 恢复栈帧到setjmp保存的环境变量那里
```

对于一个setjmp可以有多个longjmp，对于longjmp返回的时候，如果不进行任何优化的编译，全局变量、静态变量、自动变量、易失变量和寄存器变量都不受影响，不会改变，它们的值保存在存储器中；如果进行全部优化的编译（gcc -O test.c），则全局变量、静态变量和易失变量不受优化影响，其值不会改变，而自动变量和寄存器变量的值会恢复，它们保存在寄存器中

##### getrlimit和setrlimit

每个进程都有一组资源限制，可以通过这两个函数查看和更改，资源限制影响到调用进程并有子进程继承

```
#include <sys/resource.h>
int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);
// 两个函数，若成功，返回0，若出错，返回非0
```

更改资源限制三原则：

* 任何一个进程都可以将一个软限制值改为小于或等于其硬限制值
* 任何一个进程都可以降低其硬限制值，但它必须大于或等于其软限制值，这种降低对用户是不可逆的
* 只有超级用户可以提高硬限制值

常见的限制资源：

* RLIMIT_AS：进程的最大虚内存空间，字节为单位
* RLIMIT_CORE：内核转存文件的最大长度
* RLIMIT_CPU：最大允许的CPU使用时间，秒为单位。当进程达到软限制，内核将给其发送SIGXCPU
* RLIMIT_DATA：进程数据段的最大值
* RLIMIT_FSIZE：进程可建立的文件的最大长度。超出这一限制时，核心会给其发送SIGXFSZ信号
* RLIMIT_LOCKS：进程可建立的锁和租赁的最大值
* RLIMIT_MEMLOCK：进程可锁定在内存中的最大数据量，字节为单位
* RLIMIT_MSGQUEUE：进程可为POSIX消息队列分配的最大字节数
* RLIMIT_NICE：有效进程的调度优先级，可通过setpriority() 或 nice()调用设置的最大限制值
* RLIMIT_NOFILE：进程可打开的最大文件数，超出此值，将会产生EMFILE错误
* RLIMIT_NPROC：实际用户可拥有的最大子进程数
* RLIMIT_RTPRIO：进程可通过sched_setscheduler 和 sched_setparam设置的最大实时优先级
* RLIMIT_SIGPENDING：进程可以排队的最大信号数
* RLIMIT_STACK：栈的最大字节长度

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
* pause函数
* 某些ioctl函数
* 某些进程间通信函数

除非发生硬件错误，否则IO操作总能很快返回

##### 可重入函数

信号来到，进程正常指令序列临时中断，处理信号处理程序，完毕之后继续进程正常指令，但是如果正在进行malloc而被中断，且信号处理函数又进行malloc，可能会对进程造成破坏。如果在信号处理函数中保证调用安全，称为异步信号安全的。大多数函数是不可重入的，因为：

* 使用静态数据结构，如getpwnam将其结果存放到静态存储单元
* 调用malloc或free
* 标准的IO函数，如信号处理函数中的printf可能中断主程序的printf的调用

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







