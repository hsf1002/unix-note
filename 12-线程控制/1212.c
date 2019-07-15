#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

extern char **environ;

static pthread_mutex_t env_mutex;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;


/*
    以递归属性初始化互斥量
*/
static void
thread_init(void)
{
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&env_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

/*
    getenv的可重入版本
    调用者必须提供自己的缓冲区buf，这样每个线程可以使用自己不同的缓冲区避免其他线程的干扰
*/
char *
getenv_r(const char *name, char *buf, int buflen)
{
    int i;
    int len;
    int olen;

    // 多线程下解决竞争
    pthread_once(&init_done, thread_init);

    len = strlen(name);

    pthread_mutex_lock(&env_mutex);

    for (i=0; environ[i] != NULL; i++)
    {
        if ((strncmp(name, environ[i], len) == 0) && (environ[i][len] == "="))
        {
            olen = strlen(&environ[i][len+1]);
            if (olen >= buflen)
            {
                pthread_mutex_unlock(&env_mutex);
                // 无剩余空间
                return(ENOSPC);
            }
            strcpy(buf, &environ[i][len+1]);
            pthread_mutex_unlock(&env_mutex);
            return(0);
        }
    }
    pthread_mutex_unlock(&env_mutex);

    // 无此文件或目录
    return(ENOENT);
}
