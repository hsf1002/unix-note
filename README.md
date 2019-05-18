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

