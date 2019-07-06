#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>


/*
    打印进程、线程的ID
*/
void
print_ids(const char *s)
{
    pid_t pid;
    pthread_t tid;

    pid = getpid();
    tid = pthread_self();

    printf("%s pid = %lu, tid = %lu (0x%1x) \n ", s, (unsigned long)pid, (unsigned long)tid, (unsigned long)tid);
}

/*
    线程回调
*/
void *
thr_fn(void *arg)
{
    print_ids("new thread: ");
    return ((void *) 0);
}

/*
    打印进程、线程的ID

main thread:  pid = 84548, tid = 140735626785664 (0x910ab380)
 new thread:  pid = 84548, tid = 123145507332096 (0xc386000)
*/
int main(void) 
{
    int err;
    pthread_t thr;

    if ((err = pthread_create(&thr, NULL, thr_fn, NULL)) != 0)
        printf("can't create thread, err = %d \n", err);

    print_ids("main thread: ");
    sleep(1);

    exit(0);
}


