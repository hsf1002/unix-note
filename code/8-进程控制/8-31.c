#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <errno.h>


/*
    退出的通用操作
*/
void pr_exit(int status)
{
    // 正常终止子进程
    if (WIFEXITED(status))
    {
        // 获取子进程返回信息
        printf("normal termination, exit status = %d \n", WEXITSTATUS(status));
    }
    // 异常终止子进程
    else if (WIFSIGNALED(status))
    {
        // 获取子进程终止的信号编号
        printf("abnormal termination, signal number = %d%s \n", WTERMSIG(status),
        // 是否有coredump产生
#ifdef WCOREDUMP
        WCOREDUMP(status) ? "(core file generated)" : "");
#else
        ");"
#endif    
    }
    // 暂停子进程
    else if(WIFSTOPPED(status))
    {
        // 获取使子进程暂停的信号编号
        printf("child stopped, signal number = %d \n", WSTOPSIG(status));
    }
}

/*
    统计进程时间
*/
static void pr_times(clock_t real, struct tms *start, struct tms *end)
{
    static long clk = 0;

    if (clk == 0)
    {
        if ((clk = sysconf(_SC_CLK_TCK)) < 0)
        {
            ferror("sysconf error");
        }
    }

    printf(" real: %7.2f \n", real/(double)clk);
    printf(" user: %7.2f \n", (end->tms_utime - start->tms_utime)/(double)clk);
    printf(" sys: %7.2f \n", (end->tms_stime - start->tms_stime)/(double)clk);
    printf(" child user: %7.2f \n", (end->tms_cutime - start->tms_cutime)/(double)clk);
    printf(" child sys: %7.2f \n", (end->tms_cstime - start->tms_cstime)/(double)clk);
}

/*
    计算命令执行时间
*/
static void do_cmd(char *cmd)
{
    struct tms tmsstart, tmsend;
    clock_t start, end;
    int status;

    printf("\n command: %s \n", cmd);

    // 开始时间
    if ((start = times(&tmsstart)) == -1)
    {
        ferror("times error");
    }

    // 执行命令
    if ((status = system(cmd)) < 0)
    {
        ferror("execute error");
    }

    // 结束时间
    if ((end = times(&tmsend)) == -1)
    {
        ferror("times error");
    }

    pr_times(end - start, &tmsstart, &tmsend);
    pr_exit(status);
}

/*
    统计进程CPU时间

./a.out "sleep 5" "date" "man bash >/dev/null"

 command: sleep 5
 real:    5.00
 user:    0.00
 sys:    0.00
 child user:    0.00
 child sys:    0.00
normal termination, exit status = 0

 command: date
2019年 6月22日 星期六 08时52分22秒 CST
 real:    0.01
 user:    0.00
 sys:    0.00
 child user:    0.00
 child sys:    0.00
normal termination, exit status = 0

 command: man bash >/dev/null
 real:    0.14
 user:    0.00
 sys:    0.00
 child user:    0.22
 child sys:    0.01
normal termination, exit status = 0
*/
int main(int argc, char **argv)
{
    setbuf(stdout, NULL);

    for (int i=1; i<argc; ++i)
    {
        do_cmd(argv[i]);
    }

    exit(0);
}
