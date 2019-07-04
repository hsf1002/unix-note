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

