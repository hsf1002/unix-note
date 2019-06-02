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





