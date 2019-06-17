#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int globvar = 6;


/*
    vfork创建新进程的目的是exec新程序，它会保证子进程先运行，调用exec或exit之后父进程才可能被调度
./a.out 
before vfork 
child: pid = 61440, glob = 7, var = 89 
parent: pid = 61439, glob = 7, var = 89

*/
int main(int argc, char **argv)
{
    int var;
    pid_t pid;

    var = 88;
    printf("before vfork \n");

    if ((pid = vfork()) < 0)
    {
        ferror("vfork error");
    }
    else if(pid == 0)   /* child */
    {
        globvar++;
        var++;
        printf("child: ");
        printf("pid = %ld, glob = %d, var = %d \n", (long)getpid(), globvar, var);
        /* child teminated */
        _exit(0);
    }

    /* parent continues here */
    printf("parent: ");
    printf("pid = %ld, glob = %d, var = %d \n", (long)getpid(), globvar, var);

    exit(0);
}
