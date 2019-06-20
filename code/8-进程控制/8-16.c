#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


char *env_init[] = {"USER=unknown", "PATH=/tmp", NULL};

/*
    execle：指定路径，指定环境
    execlp：指定文件，继承环境

./a.out
argv[0] = echoall
argv[1] = myarg1
argv[2] = My Arg2
USER=unknown
PATH=/tmp
argv[0] = echoall
argv[1] = only 1 arg
TERM_PROGRAM=Apple_Terminal
TERM=xterm-256color
SHELL=/bin/bash
TMPDIR=/var/folders/3d/zndxynsd2777kl7gwlt6hnf80000gn/T/
Apple_PubSub_Socket_Render=/private/tmp/com.apple.launchd.Hu8ExkOr2Z/Render
TERM_PROGRAM_VERSION=404
OLDPWD=/Users/sky/work/practice/unix-note/code
TERM_SESSION_ID=A14729FF-1358-4C4E-8C59-0B13D2EC0A16
USER=sky
SSH_AUTH_SOCK=/private/tmp/com.apple.launchd.dfiBU0NqGM/Listeners
PATH=/Library/Frameworks/Python.framework/Versions/3.7/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/Users/sky/Library/Android/sdk/tools:/Users/sky/Library/Android/sdk/platform-tools://Users/sky/work/software/apache-maven-3.5.4/bin://Users/sky/software/mongodb-osx-x86_64-4.0.6/bin
PWD=/Users/sky/work/practice/unix-note/code/8-进程控制
LANG=zh_CN.UTF-8
XPC_FLAGS=0x0
XPC_SERVICE_NAME=0
HOME=/Users/sky
SHLVL=1
LOGNAME=sky
_=./a.out
*/
int main(int argc, char **argv)
{
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        ferror("first fork error");
    }
    // 子进程
    else if(pid == 0)   
    {
        // 第二次fork
        if (execle("/Users/sky/work/practice/unix-note/code/8-进程控制/echoall", "echoall", "myarg1", "My Arg2", (char *)0, env_init) < 0)
        {
            ferror("execle error");
        }
    }

    if (waitpid(pid, NULL, 0) < 0)
    {
        ferror("wait error");
    }

    if ((pid = fork()) < 0)
    {
        ferror("second fork error");
    }
    else if (pid == 0)
    {
        // mac下需要全路径
        //if (execlp("echoall", "echoall", "only 1 arg", (char *)0) < 0)
        if (execlp("/Users/sky/work/practice/unix-note/code/8-进程控制/echoall", "echoall", "only 1 arg", (char *)0) < 0)
        {
            ferror("execlp error");
        }
    }

    exit(0);
}
