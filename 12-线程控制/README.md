### 第十二章  线程控制

##### 线程限制

可以通过sysconf函数查看线程相关限制

```
PTHREAD_DESTRUCTOR_ITERATIONS: 销毁一个线程数据最大的尝试次数，通过_SC_THREAD_DESTRUCTOR_ITERATIONS查询
PTHREAD_KEYS_MAX: 一个进程可以创建的最大key的数量,通过_SC_THREAD_KEYS_MAX查询 
PTHREAD_STACK_MIN: 线程可以使用的最小的栈空间大小, 通过_SC_THREAD_STACK_MIN查询
PTHREAD_THREADS_MAX:一个进程可以创建的最大的线程数, 通过_SC_THREAD_THREADS_MAX查询
```

##### 线程属性

```
int pthread_attr_init(pthread_attr_t *attr);   
int pthread_attr_destroy(pthread_attr_t *attr); 
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

pthread_attr_init会对属性对象动态分配空间，而pthread_attr_destroy不仅会释放该空间，还会用无效值初始化属性对象，如果该属性对象被误用，导致pthread_create函数返回错误码

线程属性包括：

```
detachstate: 线程的分离状态属性
guardsize: 线程栈末尾的警戒缓冲区大小
statckaddr: 线程栈的最低地址
stacksize: 线程栈的大小
```

```
--分离状态属性--
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate); 
int pthread_attr_getdetachstate(pthread_attr_t *attr, int *detachstate);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
// detachstate可以取值PTHREAD_CREATE_DETACHED 以分离状态启动线程，或PTHREAD_CREATE_JOINABLE 正常启动，应用程序可以获取线程的终止状态
// 如果对线程终止状态不感兴趣，可以用pthread_detach设置让操作系统在线程退出时收回它所占用的资源
```

```
--线程栈缓冲区大小--
int pthread_attr_setstack(pthread_attr_t *attr,void *stackaddr, size_t stacksize);
int pthread_attr_getstack(pthread_attr_t *attr,void **stackaddr, size_t *stacksize);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
// 对进程而言，虚地址空间大小固定，而这个空间被所有线程共享，如果线程数量太多，就会减小默认的线程栈大小；或者线程的函数分配了大量的自动变量或涉及很深的栈帧，则需要的栈大小可能要比默认的大
// 如果线程栈的虚地址空间用完了，可以使用malloc或mmap作为可替代的栈分配空间，并用pthread_attr_setstack改变新线程栈的栈位置
// stackaddr为线程栈的最低内存地址，但不一定是栈的开始位置，如果CPU结构是从高地址往低地址增长，stackaddr将是栈的结尾位置，而不是开始位置
```

```
--线程栈的最低地址--
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
int pthread_attr_getguardsize(pthread_attr_t *attr, size_t *guardsize);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
// guardsize控制着线程栈末尾之后用以避免栈溢出的扩展内存大小
// guardsize设置为0，不会提供警戒缓冲区；如果修改了stackaddr，希望认为我们自己管理栈，进而使警戒缓冲区无效，相当于guardsize设置为0
```

```
--线程栈的大小--
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(pthread_attr_t *attr, size_t *stacksize);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
// 如果希望改变默认线程栈大小，而不想自己处理线程栈的分配问题，可以使用pthread_attr_setstacksize
// 设置stacksize时，不能小于PTHREAD_STACK_MIN
```

##### 同步属性

***

互斥量属性

```
int pthread_mutexattr_init(pthread_mutexattr_t *attr);  
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr); 
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

主要有三个属性

* 进程共享属性：标准可选的，运行时通过_SC_THREAD_PROCESS_SHARED查看，如果设置为PTHREAD_PROCESS_SHAERD，从多个进程彼此之间共享的内存数据块中分配的互斥量就能用于这些进程的同步；设置为PTHREAD_PROCESS_PRIVATE时，允许线程库提供更有效的互斥量实现，这是默认的情况

  ```
  int pthread_mutexattr_getpshared(const pthread_mutexattr_t *restrict attr, int *restrict pshared); 
  int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr,int pshared); 
  // 两个函数返回值：若成功，返回0，若出错，返回错误编号
  ```

* 健壮属性：与在多个进程间共享的互斥量有关，默认值是PTHREAD_MUTEX_STALLED，意味着互斥量在进程终止时不需要采取任何动作，此时使用互斥量的行为未定义，等待该互斥量的程序会被“拖住”；另一个取值PTHREAD_MUTEX_ROBUST，改变了调用pthread_mutex_lock的方式，因为必须检查3个返回值而不是之前的两个：不需要恢复的成功、需要恢复的成功、失败（如果不用健壮属性，只能检查成功或失败）

  ```
  int pthread_mutexattr_getrobust(const pthread mutexattr_t *restrict attr, int *restrict robust);
  int pthread_mutexattr_setrobust(pthread mutexattr_t *restrict attr, int robust);
  // 两个函数返回值：若成功，返回0，若出错，返回错误编号
  ```

* 类型属性：控制着互斥量的锁定特性

  ```
  互斥量类型					用途			没有解锁时再次加锁	不占用时解锁	在已解锁时解锁
  PTHREAD_MUTEX_NORMAL	标准类型，不做任何检查	     死锁	      未定义	    未定义
  PTHREAD_MUTEX_ERRORCHECK进程错误检查	            返回错误	返回错误	 返回错误
  PTHREAD_MUTEX_RECURSIVE	避免死锁	             允许	       返回错误	    返回错误
  PTHREAD_MUTEX_DEFFAULT	请求默认语义	            未定义	     未定义	   未定义
  ```

  ```
  int pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict attr,int *restrict type); 
  int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
  // 两个函数返回值：若成功，返回0，若出错，返回错误编号
  ```

  如果属性是PTHREAD_MUTEX_RECURSIVE，可以进行多次加锁，但是解锁次数和加锁次数不一致，将无法解锁

***

读写锁属性

唯一支持的属性是进程共享属性与互斥量的共享属性相同

```
int pthread_rwlockattr_init(pthread_rwlockattr_t *attr); 
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *restrict attr, int *restrict pshared);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr,int pshared);
// 四个函数返回值：若成功，返回0，若出错，返回错误编号
```

***

条件变量属性

目前支持两个属性：进程共享属性和时钟属性

```
int pthread_condattr_init(pthread_condattr_t *attr); 
int pthread_condattr_destroy(pthread_condattr_t *attr);
int pthread_condattr_getpshared(const pthread_condattr_t *restrict attr,int *restrict pshared);
int pthread_condattr_setpshared(pthread_condattr_t *attr,int pshared);
// 四个函数返回值：若成功，返回0，若出错，返回错误编号
```

时钟属性控制计算pthread_cond_timedwait函数的超时参数采用的是哪个时钟

```
int pthread_condattr_getclock(const pthread_condattr_t *restrict attr, clockid_t * restrict clock_id);
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t *clock_id);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

***

屏障属性

只支持进程共享属性

```
int pthread_barrierattr_init(pthread_barrierattr_t* attr);
int pthread_barrierattr_destroy(pthread_barrierattr_t* attr);
int pthread_barrierattr_setshared(pthread_barrierattr_t* attr,int pshared);
int pthread_barrierattr_getshared(const pthread_barrierattr_t* restrict attr,int* restrict pshared);
// 四个函数返回值：若成功，返回0，若出错，返回错误编号
```

##### 重入

如果一个函数在相同的时间点可以被多个线程安全的调用，该函数式线程安全的；如果一个函数对多个线程是可重入的，这个函数就是线程安全的；如果函数对异步信号处理程序的重入是安全的，该函数式异步信号安全的；下表列出的是POSIX.1不能保证线程安全的函数

```
asctime	ecvt	gethostent	getutxline	putc_unlocked
basename	encrypt	getlogin	gmtime	putchar_unlocked
catgets	endgrent	getnetbyaddr	hcreate	putenv
crypt	endpwent	getnetbyname	hdestroy	pututxline
ctime	endutxent	getopt	hsearch	rand
dbm_clearerr	fcvt	getprotobyname	inet_ntoa	readdir
dbm_close	ftw	getprotobynumber	L64a	setenv
dbm_delete	getcvt	getprotobynumber	lgamma	setgrent
dbm_error	getc_unlocked	getprotoent	lgammaf	setkey
dbm_fetch	getchar_unlocked	getpwent	lgammal	setpwent
dbm_firstkey	getdate	getpwnam	localeconv	setutxent
dbm_nextkey	getenv	getpwuid	lrand48	strerror
dbm_open	getgrent	getservbyname	mrand48	strtok
dbm_store	getgrgid	getservbyport	nftw	ttyname
dirname	getgrnam	getservent	nl_langinfo	unsetenv
dlerror	gethostbyaddr	getutxent	ptsname	wcstombs
drand48	gethostbyname	getutxid	ptsname	ectomb
```

以线程安全的方式管理FILE对象的方法：

```
#include <stdio.h>

int ftrylockfile(FILE *filehandle);
// 若成功，返回0，若不能获取锁，返回非0
void flockfile(FILE *filehandle);
void funlockfile(FILE *filehandle);
```

##### 线程特定数据

线程私有数据，除了寄存器外，一个线程没办法阻止另一个线程访问它的数据，管理线程特定数据可以提高线程间的数据独立性

```
int pthread_key_create(pthread_key_t *keyp, void (* _Nullable)(void *));
int pthread_key_delete(pthread_key_t keyp);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

* keyp可以被进程中所有线程使用，但是每个线程把这个键与不同的线程特定数据地址进行关联

* pthread_key_create可以为该键关联一个可选的析构函数，当线程调用pthread_exit或执行返回正常退出时，析构函数就会调用，线程取消时，只有在最后的清理处理程序返回之后，析构函数才会被调用，如果线程调用了exit、_exit、_Exit或abort，或其他非正常退出时，不会调用西沟函数

* pthread_key_delete用来取消键与线程特定数据之间的关联，调用它并不会激活该键关联的析构函数

* 需要确保分配的键不会由于初始化阶段的竞争而发生变动，这样会导致有些线程看到的是一个键值，其他线程看到的是不同的键值，解决竞争的办法是使用pthread_once

  ```
  pthread_once_t initflag = PTHREAD_ONCE_INIT;
  int pthread_once(pthread_once_t *initflag, void (* _Nonnull)(void));
  // 若成功，返回0，若出错，返回错误编号
  // initflag必须是非本地变量(即全局或静态变量)，且必须用PTHREAD_ONCE_INIT初始化
  ```

* 键一旦创建，就可以通过pthread_setspecific把键和线程特定数据关联起来

  ```
  void* _Nullable pthread_getspecific(pthread_key_t key);
  // 返回线程特定数据，若没有值与该键关联，返回NULL
  int pthread_setspecific(pthread_key_t key, const void * _Nullable value);
  // 若成功，返回0，若出错，返回错误编号
  ```

##### 取消选项

有两个属性没有包含在pthread_attr_t中，可取消状态和可取消类型，它们影响着线程在响应pthread_cancel调用时的行为

```
int pthread_setcancelstate(int state, int * oldstate);
// 若成功，返回0，若出错，返回错误编号
#define PTHREAD_CANCEL_ENABLE        0x01  // 下个取消点生效
#define PTHREAD_CANCEL_DISABLE       0x00  // 取消推迟
```

调用pthread_cancel并不等待线程终止，默认情况下，线程在取消请求发出后还是继续运行，直到线程达到某个取消点，POSIX定义了取消点和可选取消点的函数

```
// 一定会执行取消点的函数
accept	mq_timedsend	putpmsg	sigsuspend
aio_suspend	msgrcv	pwrite	sigtimedwait
clock_nanosleep	msgsnd	read	sigwait
close	msync	readv	sigwaitinfo
connect	nanosleep	recv	sleep
creat	open	recvfrom	system
fcnt12	pause	recvmsg	tcdrain
fsync	poll	select	usleep
getmsg	pread	sem_timedwait	wait
getpmsg	pthread_cond_timedwait	sem_wait	waitid
lockf	pthread_cond_wait	send	waitpid
mq_receive	pthread_join	sendmsg	write
mq_send	pthread_testcancel	sendto	writev
mq_timedreceive	putmsg	sigpause	
```

线程启动时默认的可取消状态是PTHREAD_CANCEL_ENABLE，设置为PTHREAD_CANCEL_DISABLE时，调用pthread_cancel并不会杀死线程，相反，取消请求对这个线程来说还处于挂起状态，当取消状态再次变成PTHREAD_CANCEL_ENABLE时，线程将在下个取消点对所有挂起的取消请求进行处理

可以使用pthread_testcancel添加自己的取消点

```
void pthread_testcancel(void);
```

取消类型也就是推迟取消，调用pthread_cancel后，在线程到达取消点之前，并不会出现真正的取消，可以调用pthread_setcanceltype来修改取消类型

```
int pthread_setcanceltype(int type, int *oldtype);
// 若成功，返回0，若出错，返回错误编号
```

##### 线程与信号

把线程引入编程范型，使得信号的处理变得更加复杂。单个线程可以阻止某些信号，当当某个线程修改了与某个给定信号相关的处理行为后，所有的线程都必须共享这个处理行为的改变。如一个线程忽略某个信号，则另一个线程就可以通过两种方式撤销上述线程的信号选择：恢复信号的默认处理行为，或为信号设置一个新的信号处理程序。如果一个信号与硬件故障有关，则该信号一般会发生到引起该事件的线程，其他信号则被发送到任意一个线程。进程中使用sigprocmask阻止信号发送，线程中则使用pthread_sigmask

```
int pthread_sigmask(int how, const sigset_t *restrict set, sigset_t *restrict oset);
// 若成功，返回0，若出错，返回错误编号
```

* 工作方式与sigprocmask基本相同，how的取值
  * SIG_BLOCK：把信号集添加到线程信号屏蔽字中
  * SIG_SETMASK：用信号集替换线程的信号屏蔽字
  * SIG_UNBLOCK：从线程信号屏蔽字中移除信号集

* 如果oset不为空，则获取线程的信号屏蔽字并保存到oset
* 如果set不为空，则设置线程的信号屏蔽字为set，如果set为空，oset不为空，则how被忽略

可以通过调用sigwait等待一个或多个信号的出现

```
int sigwait(const sigset_t *set, int *signop);
// 若成功，返回0，若出错，返回错误编号
```

* set指定了等待的信号集
* signop指向的整数将包含发送信号的数量
* 如果信号集中某个信号在调用sigwait时处于挂起状态，那么sigwait无阻塞的返回，返回之前，从进程中移除那些处于挂起的信号；为了避免错误行为发生，线程在调用sigwait前，必须阻塞那些它正在等待的信号；sigwait会原子的取消信号集的阻塞状态，直到新的信号被递送，返回之前，sigwait将恢复线程的信号屏蔽字

发送信号给进程，可以调用kill，发送信号给线程，可以调用pthread_kill

```
int pthread_kill(pthread_t thread, int signo);
// 若成功，返回0，若出错，返回错误编号
```

可以传0给signo检查线程是否存在，如果信号的默认处理动作是终止该进程，那么把信号传递给某个线程仍然会杀死整个进程

##### 线程与fork

在多线程的进程中，在fork返回和子进程调用exec之间，子进程只能调用异步信号安全的函数。fork后，子进程继承了整个地址空间的副本，包括每个互斥量、读写锁和条件变量，如果父进程包含多个线程，子进程在fork返回后，如果不马上调用exec的话，需要清理锁状态。可以通过pthread_atfork建立fork处理程序，最多可以安装三个帮助清理锁状态的函数

```
int pthread_atfork(void (*prepare)(void), void (*parent)(void),void (*child)(void));
// 若成功，返回0，若出错，返回错误编号
```

* prepare：在fork创建子进程前调用，任务是获取父进程定义的所有锁
* parent：在fork创建子进程后，返回之前在父进程的上下文中调用，任务是对prepare获取的所有锁进行解锁
* child：在fork返回之前，在子进程上下文中调用，任务是释放prepare获取的所有锁

##### 线程与IO

以下两个操作，将导致两个线程读取到同一内容

```
// thread A
lseek(fd, 300, SEEK_SET);
read(fd, buf1, 100);

// thread B
lseek(fd, 700, SEEK_SET);
read(fd, buf2, 100);
```

为了解决此问题， 可以使用pread，使得偏移量的设定和读取成为一个原子操作

```
// thread A
pread(fd, buf1, 100, 300);

// thread B
pread(fd, buf2, 100, 700);
```

