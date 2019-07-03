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