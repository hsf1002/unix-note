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
echo $SHELL
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
