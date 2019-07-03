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
