
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