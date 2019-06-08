#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXLINE  100

static void sig_int(int signo)
{
    printf("sig_int: interrupted! signo = %d \n", signo);
}

/*
    SIGINT的系统默认动作是终止进程
*/
int main(int argc, char *argv[])
{
    char buf[MAXLINE];
    pid_t pid;
    int status;

    if (signal(SIGINT, sig_int) == SIG_ERR)
    {
        ferror("signal register error! \n");
    }

    // 区别于$
    printf("%% ");

    while (fgets(buf, MAXLINE, stdin) != NULL)
    {
        if (buf[strlen(buf) - 1] == '\n')
        {
            buf[strlen(buf) - 1] = 0;   /* replace newline with null */
        }

        if ((pid = fork()) < 0)
        {
            ferror("fork error");
        }
        /* child */
        else if (pid == 0)
        {
            execlp(buf, buf, (char *)0);
            printf("couldnot execute: %s \n", buf);
            exit(127);
        }
        
        /* parent */
        if ((pid = waitpid(pid, &status, 0)) < 0)
        {
            printf("waitpid error! \n");
        }

        printf("%% ");
    }

    exit(0);
}
