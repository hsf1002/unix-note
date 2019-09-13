#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/msg.h>

#define PATHNAME "."
#define PROJ_ID 0x6666

#define SERVER_TYPE 1
#define CLIENT_TYPE 2

struct msgbuf
{
    long mtype;
    char mtext[1024];
};

/**
 * 打开或创建消息队列
 */
static int msgq_init(int flags)
{
    // 获取关键字
    key_t key;
    int msgq_id;

    if (-1 == (key = ftok(PATHNAME, PROJ_ID)))
    {
        perror("ftok error");
        return(-1);
    }

    // 创建或打开消息队列
    if (-1 == (msgq_id = msgget(key, flags)))
    {
        perror("msgget error");
        return(-2);
    }

    return msgq_id;
}

/*
    创建
*/
int msgq_create()
{
    return msgq_init(IPC_CREAT|IPC_EXCL|0666);
}

/*
    打开
*/
int msgq_open()
{
    return msgq_init(IPC_CREAT);
}

/*
    销毁
*/
int msgq_destroy(int msgq_id)
{
    int ret;

    if ((ret = msgctl(msgq_id, IPC_RMID, NULL)) == -1)
    {
        perror("msgq destry error");
    }

    return ret;
}

/*
    发送数据
*/
int msgq_send(int msgq_id, long who, char *msg)
{
    int ret;
    struct msgbuf buf;
    buf.mtype = who;
    strncpy(buf.mtext, msg, strlen(msg));

    if ((ret = msgsnd(msgq_id, (void *)&buf, sizeof(buf.mtext), 0)) == -1)
    {
        perror("msgsnd error");
    }

    return ret;
}

/*
    接收数据
*/
ssize_t msgq_receive(int msgq_id, long rev_type, char out[])
{
    struct msgbuf buf;
    ssize_t ret;

    if (-1 == (ret = msgrcv(msgq_id, (void *)&buf, sizeof(buf.mtext), rev_type, 0)))
    {
        perror("smgrcv error");
        return(-1);
    }
    strncpy(out, buf.mtext, strlen(buf.mtext));
    return 0;
}

/*
    消息队列的简单使用
*/
int 
main(int argc, char *argv[]) 
{
    pid_t pid;
    
    if ((pid = fork()) < 0)
    {
        perror("fork error \n");
        exit(1);
    }
    // 父进程
    else if (pid > 0)
    {
        int msgq_id = msgq_create();
        char buf[64] = {0};

        // 服务端先发送一条消息
        msgq_send(msgq_id, SERVER_TYPE, "server-to: hello");
        sleep(2);
        // 等待2秒再从客户端接收一条消息
        msgq_receive(msgq_id, CLIENT_TYPE, buf);
        printf("server-from-client: buf = %s \n", buf);
        msgq_destroy(msgq_id);

        exit(0);
    }
    // 子进程
    else
    {
        int msgq_id = msgq_open();
        char buf[64] = {0};

        // 先从服务端读取一条消息
        msgq_receive(msgq_id, SERVER_TYPE, buf);
        printf("client-from-server: buf = %s \n", buf);
        // 再给服务端发送一条消息
        msgq_send(msgq_id, CLIENT_TYPE, "client-to: sky");
        // 无需销毁消息队列

        exit(0);
    }
    
    exit(0);
}

