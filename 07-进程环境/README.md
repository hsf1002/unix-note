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
