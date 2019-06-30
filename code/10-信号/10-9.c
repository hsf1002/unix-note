#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>


static void 
sig_alarm1(int signo)
{
    /* nothing to do, just return to wake up the pause */
}

/*
    sleep的第一种实现，存在三个问题
    1. 如果在调用sleep1之前，设置了闹钟，则被sleep1中第一次alarm擦除，可用以下方法更改：
        检查第一次调用alarm的返回值，如果小于本次调用的参数，只需等待已有闹钟超时
        如果大于本地调用的参数，则在sleep1返回前，重置闹钟，使其在之前设定的时间再次发生超时
    2. 修改了对于SIGALRM的配置，需要保存signal的返回值，在返回前恢复原配置
    3. 第一次调用alarm和pause之间有一个竞争条件，在系统繁忙时有可能alarm在pause前超时并调用信号处理程序
        如果如此，在调用pause后，如果没有捕捉到其他信号，调用者将永远被挂起，可用以下方法更改：
        1）. setjmp
        2）. sigprocmask和sigsuspend（10.19说明）
*/
int sleep1(int seconds)
{
    if (signal(SIGALRM, sig_alarm1) == SIG_ERR)
        return seconds;

    // 开启定时
    alarm(seconds);
    // 一般很快会调用，等待SIGALRM发生时，唤醒
    pause();
    
    return(alarm(0));
}


static jmp_buf env_alrm;

static void 
sig_alarm2(int signo)
{
    longjmp(env_alrm, 1);
}

/*
    sleep的第二种实现，使用setjmp解决第三个问题，还存第一第二个问题
*/
int sleep2(int seconds)
{
    if (signal(SIGALRM, sig_alarm2) == SIG_ERR)
        return seconds;

    if (setjmp(env_alrm) == 0)
    {
        // 开启定时
        alarm(seconds);
        // 下个捕获信号，即SIGALRM发生时，才会唤醒
        pause();
    }
    
    // 关闭闹钟，返回剩余时间
    return(alarm(0));
}

static void
sig_int(int signo)
{
    int i, j;
    volatile int k; // volatile阻止优化编译去掉循环

    printf("\n sig_int starting... \n");

    for (i=0; i<300000; i++)
    {
        for (j=0; j<4000; j++)
            k += i*j;
    }

    printf("sig_int finished \n");
}

/*
    如果SIGALRM中断了某个其他信号处理程序，则调用longjmp会提早终止该信号处理程序

./a.out 
^C
 sig_int starting... 
sleep2 returned, unsleep = 0

*/
int main(void)
{
    unsigned int unsleep;

    if (signal(SIGINT, sig_int) == SIG_ERR)
        perror("signal(SIGINT) error \n");

    // sleep2中的longjmp使得另一个信号处理程序sig_init提早终止，即使它未完成也会如此
    unsleep = sleep2(3);
    printf("sleep2 returned, unsleep = %u \n", unsleep);
    exit(0);
}
