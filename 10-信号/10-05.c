#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>

static void my_alarm(int signo)
{
    struct passwd *rootptr;

    printf("in signal handler \n");
    if (rootptr = getpwnam("root") == NULL)
        ferror("getpwnam(root) error ");
	else
		printf("return value, pw_name = %s \n", rootptr->pw_name);    
    // 1秒钟执行一次my_alarm
    alarm(1);
}

/*
    getpwnam是不可重入函数，主程序和信号处理程序都会调用，可能造成返回结果具有随机性

./a.out 
段错误 (核心已转储)

for里面添加sleep(1)之后：
./a.out 
return value, pw_name = hefeng 
in signal handler 
段错误 (核心已转储)


    主程序已经调用getpwnam，当free时，信号处理函数中断了它，也调用getpwnam，并且同时进行free，
    使得数据结构出现损坏，因此运行几秒就会产生SIGSEGV信号终止，主程序可能仍能正常运行，但是返回值有时正确，有时错误
*/
int main(int argc, char **argv)
{
    struct passwd *ptr;

    signal(SIGALRM, my_alarm);
    alarm(1);

    for (;;)
    {
        if ((ptr = getpwnam("hefeng")) == NULL)
            ferror("getpwnam error");
		else        
            printf("return value, pw_name = %s \n", ptr->pw_name);
		
		if (strcmp(ptr->pw_name, "hefeng") != 0)
			printf("return value corrupted!\n", ptr->pw_name);
		sleep(1);
    }
}

