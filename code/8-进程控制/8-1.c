#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int globvar = 6;
char buf[] = "a write to stdout \n";

/*
    fork前后对于全局变量和局部变量的影响

./a.out 
a write to stdout 
before fork 
now child: pid = 61223, glob = 7, var = 89 
now parent: pid = 61222, glob = 6, var = 88    
*/
int main(void)
{
    int var;
    pid_t pid;

    var = 88;

    if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1)
    {
        ferror("write error");
    }
    printf("before fork \n");   /* here we don't flush stdout */

    if ((pid = fork()) < 0)
    {
        ferror("fork error");
    }
    else if (pid == 0)      /* child */
    {
        globvar++;
        var++;
        printf("now child: ");
    }
    else                    /* parent */
    {
        sleep(2);
        printf("now parent: ");
    }
    
    printf("pid = %ld, glob = %d, var = %d \n", (long)getpid(), globvar, var);

    exit(0);
}
