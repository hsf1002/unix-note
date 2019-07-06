### 第十一章  线程

##### 线程概念

* 通过为每种事件类型分配单独的处理线程，可以简化处理异步事件的代码
* 多个进程必须使用操作系统提供的复杂机制才能实现内存和文件描述符的共享
* 有些问题可以分解从而提高程序的吞吐量
* 交互的程序通过多线程可以改善响应时间

每个线程都包含表示执行环境所必须的信息，其中有进程中标识线程的线程ID、一组寄存器值、栈、调度优先级和策略、信号屏蔽字、errno变量以及线程私有数据

##### 线程标识

线程ID只有在它所属的进程上下文中才有意义，用pthread_t这个类型标识的非负整数，必须用函数比较两个线程ID：

```
#include <pthread.h>

int pthread_equal(pthread_t tid1, pthread_t tid2);
// 若相等，返回非0，否则，返回0
```

线程可以通过调用pthread_self来获取自身的线程ID：

```
#include <pthread.h>

pthread_t pthread_self(void);
// 返回值：调用线程的线程ID
```

##### 线程创建

```
int pthread_create(pthread_t * __restrict tidp,
		const pthread_attr_t * __restrict attr,
		void * (*start_rtn)(void *),
		void * __restrict arg);
// 若成功，返回0，若失败，返回错误编号		
```

* 若成功返回，新创建线程的ID会被设置为tidp指向的内存单元
* 新线程从start_rtn开始运行，该函数只有一个无类型指针参数，如果需要多个参数，需要将他们放到一个结构体中
* 线程创建后，不能保证新线程先运行，还是调用线程先运行

##### 线程终止

如果进程中任意线程调用了exit、_Exit、或 _exit，则整个进程就会终止，如果收到了默认动作是终止进程的信号，也是类似，单个线程有三种方式退出：

* 从启动例程中返回，返回值是线程的退出码
* 被同一进程的其他线程取消
* 调用pthread_exit

```
void pthread_exit(void * rval_ptr);
```

rval_ptr是无类型指针，与传给启动例程的单个参数类似

```
int pthread_join(pthread_t thread, void ** rval_ptr)
// 若成功，返回0，若出错，返回错误编号
```

如果某个线程调用了pthread_join，则一直阻塞，直到指定的线程thread调用pthread_exit、被取消（rval_ptr指定的内存单元设置为PTHREAD_CANCELED）或从启动例程返回（rval_ptr包含返回码）

线程可以通过pthread_cancel来请求取消同一进程中的其他线程

```
int pthread_cancel(pthread_t tid);
// 若成功，返回0，若出错，返回错误编号
```

默认情况下，会使得tid的线程如同调用了参数为PTHREAD_CANCELED的pthread_exit函数，但是线程可以选择忽略取消或者控制如何被取消，pthread_cancel并不实际取消，仅仅提出请求

线程清理程序：退出时调用的函数，与进程退出时可以用atexit函数是类似的，一个线程可以注册多个清理处理程序，它们的执行顺序与注册顺序相反

```
void pthread_cleanup_push(void (*rtn)(void *), void *arg);
void pthread_cleanup_pop(int execute);
// macOS中以宏实现
```

当执行以下动作时，清理函数rtn由pthread_cleanup_push调度：

* 调用pthread_exit时
* 相应取消请求时
* 用非零execute参数调用pthread_cleanup_pop时
* 如果execute参数为0，则清理函数不被调用
* 如果线程是通过它的启动例程中返回而终止，它的清理程序就不会被调用

进程和线程之间的相似之处

```
进程原语        线程原语            描述
fork       pthread_create        创建新的控制流
exit       pthread_exit          从控制流中退出
waitpid    pthread_join          得到退出状态
atexit     pthread_cancel_push   注册退出函数
getpid     pthread_self          获取控制流ID
abort      pthread_cancel        请求控制流的非正常退出
```

可以调用pthread_detach分离线程

```
int pthread_detach(pthread_t tod);
// 若成功，返回0，否则，返回错误编码
```

* 默认线程的终止状态会保存直到调用pthread_join
* 如果线程已经被分离，其底层存储资源可以在线程终止时立即被收回
* 如果线程已经被分离，调用pthread_join会产生未定义行为

##### 线程同步

当一个线程可以修改的变量，其他线程可以读或写的时候，需要对这些线程进行同步。在变量修改时间多于一个存储器访问周期的处理器解构中，当读与写这两个周期交叉时，这种不一致就会出现

##### 互斥量

互斥量本质上是一把锁，使用前必须初始化，或者用静态分配的常量PTHREAD_MUTEX_INITIALIZER，或者用pthread_mutex_init进行初始化，用pthread_mutex_destroy释放内存

```
#include <pthread.h>

int pthread_mutex_init(pthread_mutex_t * __restrict mutex,
		const pthread_mutexattr_t * __restrict attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

```
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
// 三个函数返回值：若成功，返回0，若出错，返回错误编号
```

如果线程不希望阻塞，调用pthread_mutex_trylock尝试对互斥量加锁，如果成功锁住，不会阻塞直接返回0，否则返回EBUSY

##### 避免死锁

如果试图对一个互斥量加锁两次，其自身会陷入死锁状态；或者程序中有一个以上的互斥量，两个线程互相请求对方所拥有的资源时，也会死锁。可以通过仔细控制互斥量加锁的顺序来避免死锁，如所有线程总是在对互斥量B加锁前先锁住互斥量A，但是对互斥量的排序有时候比较困难，可以使用pthread_mutex_trylock避免死锁，如果不能获取锁，先释放已经占有的资源，做好清理工作，过一段时间再试

##### 带有超时的互斥量

pthread_mutex_timedlock互斥量原语运行绑定阻塞时间

```
#include<pthread.h>

int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex,
							const struct timespec *restrict tsptr);
// 若成功，返回0，若出错，返回错误编号
```

超时指的是绝对时间，可以使用clock_gettime（并非所有平台都支持），或通过如下方式获取：

```
#include <sys/time.h>

void
maketimeout(struct timespec *tsp, long minutes)
{
    struct timeval now;

    gettimeofday(&now, NULL);
    tsp->tv_sec = now.tv_sec;
    tsp->tv_nsec = now.tv_usec * 1000;  /* usec -> nsec */
    
    tsp->tv_sec += minutes * 60;
}
```

##### 读写锁

与互斥量类似，允许更高的并行性。读写锁有三种状态：读模式下加锁、写模式下加锁和不加锁。一次一个线程可以占有写模式的读写锁，但多个线程可以同时占有读模式的读写锁。当是写加锁模式时，所有试图对这个锁加锁的线程都会阻塞，当是读加锁模式时，所有试图以读模式进行加锁的线程都可以得到访问权。读写锁非常适合对数据结构读的次数远大于写的情况。也称为共享互斥锁，当是读模式时，是共享的，当是写模式时，是互斥的

```
int pthread_rwlock_init(pthread_rwlock_t * __restrict rwlock,
						const pthread_rwlockattr_t * __restrict attr);
int pthread_rwlock_destroy(pthread_rwlock_t * rwlock);
// 两个函数的返回值：若成功，返回0，若出错，返回错误编号
```

```
int pthread_rwlock_rdlock(pthread_rwlock_t *); // 以读模式加锁
int pthread_rwlock_wrlock(pthread_rwlock_t *); // 以写模式加锁
int pthread_rwlock_unlock(pthread_rwlock_t *); // 不管以何种模式加锁，以此解锁
// 三个函数返回值：若成功，返回0，若出错，返回错误编号
```

```
int pthread_rwlock_tryrdlock(pthread_rwlock_t *);
int pthread_rwlock_trywrlock(pthread_rwlock_t *);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

##### 带有超时的读写锁

```
int pthread_rwlock_timedrdlock (pthread_rwlock_t *__restrict __rwlock,
                                const struct timespec *__restrict __abstime);
int pthread_rwlock_timedwrlock (pthread_rwlock_t *__restrict __rwlock,
                                const struct timespec *__restrict __abstime);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

与pthread_mutex_timedlock一样，超时指的是绝对时间

##### 条件变量

条件变量与互斥量一起使用时，允许线程以无竞争的方式等待特定的条件发生，使用前必须对它进行初始化，可以用PTHREAD_COND_INITIALIZER或pthread_cond_init进行初始化

```
int pthread_cond_init(pthread_cond_t * __restrict cond,
					const pthread_condattr_t * _Nullable __restrict attr);
int pthread_cond_destroy(pthread_cond_t *cond);		
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

阻塞，等待唤醒：

```
int pthread_cond_wait(pthread_cond_t * __restrict cond, pthread_mutex_t * __restrict mutex);
int pthread_cond_timedwait(pthread_cond_t * __restrict cond, pthread_mutex_t * __restrict mutex, const struct timespec * _Nullable __restrict tsptr);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

唤醒：

```
int pthread_cond_signal(pthread_cond_t *cond);	// 至少唤醒一个等待该条件的线程
int pthread_cond_broadcast(pthread_cond_t *cond); // 唤醒所有等待该条件的线程
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

##### 自旋锁

与互斥量类似，但是不通过休眠使进程阻塞，而是在获取锁之前一直处于忙等阻塞状态，可用于以下情况：持有时间短而且线程并不希望在重新调度上花太多成本。使用前必须对它进行初始化，可以用PTHREAD_SPINLOCK_INITIALIZER或pthread_spin_init进行初始化

```
int pthread_spin_init (pthread_spinlock_t *lock, int pshared);
int pthread_spin_destroy (pthread_spinlock_t *lock);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

pshared表示进程共享属性：如果设置为PTHREAD_PROCESS_SHARED，则自旋锁能被可以访问锁底层内存的线程获取，即使那些线程属于不同的进程；如果设置为PTHREAD_PROCESS_PRIVATE，只能被进程内部的线程访问

```
int pthread_spin_trylock (pthread_spinlock_t *lock);
int pthread_spin_unlock (pthread_spinlock_t *lock);
int pthread_spin_lock (pthread_spinlock_t *lock);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

不用调用在持有自旋锁情况下可能进入休眠状态的函数，会浪费CPU资源，因为其他线程需要获取自旋锁需要等待的时间延长了

##### 屏障

屏障，或栅栏，是用户协调多个线程并行工作的同步机制，允许每个线程等待，直到所有线程都达到某一点，然后继续执行，pthread_join就是屏障的一种

```
int pthread_barrier_init(pthread_barrier_t *restrict barrier, 
						const pthread_barrierattr_t *restrict attr, unsigned count);
int pthread_barrier_destroy(pthread_barrier_t *barrier);
// 两个函数返回值：若成功，返回0，若出错，返回错误编号
```

```
int pthread_barrier_wait(pthread_barrier_t *barrier);
// 若成功，返回0或PTHREAD_BARRIER_SERIAL_THREAD，若出错，返回错误编号
```

调用pthread_barrier_wait的线程在屏障计数未满足条件，进入休眠状态，否则，所有线程都被唤醒

