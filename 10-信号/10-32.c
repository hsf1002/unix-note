#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

/*

./a.out
psignal--------
SIGUSR1: User defined signal 1
SIGSEGV: Segmentation fault
SIGHUP: Hangup
strsignal--------
SIGUSR1: User defined signal 1: 30
SIGSEGV: Segmentation fault: 11
SIGHUP: Hangup: 1

sys_siglist--------
SIGUSR1: User defined signal 1
SIGSEGV: Segmentation fault
SIGHUP: Hangup

psiginfo-------- macOS not support

*/
int main(void) 
{
    printf("psignal-------- \n");
    psignal(SIGUSR1, "SIGUSR1");
    psignal(SIGSEGV, "SIGSEGV");
    psignal(SIGHUP, "SIGHUP");

    printf("strsignal-------- \n");
    printf("SIGUSR1: %s \n", strsignal(SIGUSR1));
    printf("SIGSEGV: %s \n", strsignal(SIGSEGV));
    printf("SIGHUP: %s \n\n", strsignal(SIGHUP));

    printf("sys_siglist-------- \n");
    printf("SIGUSR1: %s \n", sys_siglist[SIGUSR1]);
    printf("SIGSEGV: %s \n", sys_siglist[SIGSEGV]);
    printf("SIGHUP: %s \n\n", sys_siglist[SIGHUP]);

    printf("psiginfo-------- macOS not support \n");

    exit(0);
}
