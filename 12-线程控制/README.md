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