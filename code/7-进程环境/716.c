#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>


#define doit(name)  pr_limits(#name, name)

static void pr_limits(char *, int);

/*
    进程资源限制函数getrlimit和setrlimit

./a.out
RLIMIT_AS       (infinite)  (infinite)
RLIMIT_CORE              0  (infinite)
RLIMIT_CPU      (infinite)  (infinite)
RLIMIT_DATA     (infinite)  (infinite)
RLIMIT_FSIZE    (infinite)  (infinite)
RLIMIT_MEMLOCK  (infinite)  (infinite)
RLIMIT_NPROC           709        1064
RLIMIT_RSS      (infinite)  (infinite)
RLIMIT_STACK       8388608    67104768
RLIMIT_NOFILE          256  (infinite)
*/
int main(int argc, char *argv[])
{
#ifdef RLIMIT_AS
    doit(RLIMIT_AS);
#endif    
    doit(RLIMIT_CORE);
    doit(RLIMIT_CPU);
    doit(RLIMIT_DATA);
    doit(RLIMIT_FSIZE);
#ifdef RLIMIT_MEMLOCK    
    doit(RLIMIT_MEMLOCK);
#endif    
#ifdef RLIMIT_MSGQUEUE  
    doit(RLIMIT_MSGQUEUE);
#endif
#ifdef RLIMIT_NICE  
    doit(RLIMIT_NICE);
#endif
#ifdef RLIMIT_NPROC      
    doit(RLIMIT_NPROC);
#endif
#ifdef RLIMIT_NPTS      
    doit(RLIMIT_NPTS);
#endif
#ifdef RLIMIT_RSS      
    doit(RLIMIT_RSS);
#endif
#ifdef RLIMIT_SBSIZE      
    doit(RLIMIT_SBSIZE);
#endif
#ifdef RLIMIT_SIGPENDING      
    doit(RLIMIT_SIGPENDING);
#endif
#ifdef RLIMIT_SWAP      
    doit(RLIMIT_SWAP);
#endif
#ifdef RLIMIT_VMEM      
    doit(RLIMIT_VMEM);
#endif    
    doit(RLIMIT_STACK);
    doit(RLIMIT_NOFILE);

    return(0);
}

static void pr_limits(char *name, int resource)
{
    struct rlimit limit;
    unsigned long long lim;

    if (getrlimit(resource, &limit) < 0)
    {
        printf("getrlimit error for %s \n", name);
    }

    printf("%-14s  ", name);

    if (limit.rlim_cur == RLIM_INFINITY)
    {
        printf("(infinite)  ");
    }
    else
    {
        lim = limit.rlim_cur;
        printf("%10lld  ", lim);
    }

    if (limit.rlim_max == RLIM_INFINITY)
    {
        printf("(infinite)  ");
    }
    else
    {
        lim = limit.rlim_max;
        printf("%10lld  ", lim);
    }
    
    putchar((int)'\n');
}
