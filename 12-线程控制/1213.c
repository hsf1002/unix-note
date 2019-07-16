#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define MAXSTRINGSZ     4096

extern char **environ;

static pthread_key_t key;
static pthread_mutex_t env_mutex;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;


/*
    创建一个线程特定数据，析构函数为free，只有线程特定数据非空时，free才会调用
*/
static void
thread_init(void)
{
    pthread_key_create(&key, free);
}

/*
    getenv的另一种实现：使用线程特定数据来维护每个线程的数据缓冲区副本，用于存储各自的返回字符串
    该版本是线程安全的，但不是异步信号安全的
    也不是可重入的，因为malloc不可重入，且malloc本身也不是异步信号安全的
*/
char *
getenv_r(const char *name)
{
    int i;
    int len;
    char *envbuf;

    // 多线程下解决竞争，确保只创建了一个键
    pthread_once(&init_done, thread_init);

    pthread_mutex_lock(&env_mutex);

    // 根据key获取线程特定数据
    envbuf = (char *)pthread_getspecific(key);

    if (NULL == envbuf)
    {
        envbuf =  malloc(MAXSTRINGSZ);
        if (NULL == envbuf)
        {
            pthread_mutex_unlock(&env_mutex);
            return(NULL);
        }
        // 设置线程特定数据的值
        pthread_setspecific(key, envbuf);
    }

    len = strlen(name);

    for (i=0; environ[i] != NULL; i++)
    {
        if ((strncmp(name, environ[i], len) == 0) && (environ[i][len] == "="))
        {
            strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ-1);
            pthread_mutex_unlock(&env_mutex);
            return(envbuf);
        }
    }
    pthread_mutex_unlock(&env_mutex);

    return(NULL);
}

