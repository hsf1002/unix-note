#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAXLINE 1024
#define PATHNAME "/tmp/myfifo"


/*
    FIFO命名管道：父进程首先在磁盘上创建一个命名管道文件，然后创建两个子进程后退出。每个子进程都对管道文件进行一次读
    和一次写的动作，然后子进程退出
*/
int 
main(int argc, char *argv[]) 
{
    int fd = -1;
    pid_t pid;
    char line[MAXLINE];

    // 创建FIFO命名管道
    if (mkfifo(PATHNAME, S_IFIFO|0644) < 0)
    {
        perror("mkfifo error \n");
        exit(1);
    }
    
    fflush(NULL);

    // if (setvbuf(stdin, NULL, _IONBF, 0) != 0)
    //     perror("set stdin nobuf error \n");

    // if (setvbuf(stdout, NULL, _IONBF, 0) != 0)
    //     perror("set stdout nobuf error \n");

    if ((pid = fork()) < 0)
    {
        perror("fork error \n");
        exit(1);
    }
    // 父进程
    else if (pid > 0)
    {
        if ((pid = fork()) < 0)
        {
            perror("fork error in parent \n");
            exit(1);
        }
        // 父进程：创建完毕之后退出
        else if (pid > 0)
        {
            exit(0);
        }
        // 第二个子进程：与第一个子进程没有亲缘关系
        else
        {
            if ((fd = open(PATHNAME, O_RDONLY)) < 0)
            {
                perror("open error \n");
                exit(1);
            }
            // 2. 读出hello，并打印
            if ((read(fd, line, MAXLINE)) > 0)
            {
                printf("second child: %s\n", line);
            }
            // 3. 写入world
            //write(fd, "world!", 8);
            close(fd);
            //system("sync");
            //system("sync");

            exit(0);
        }
    }
    // 第一个子进程
    else
    {
        if ((fd = open(PATHNAME, O_RDWR)) < 0)
        {
            perror("open error \n");
            exit(1);
        }
        // 1. 先写入hello
        write(fd, "hello world", 13);
        sleep(1);
        printf("first write success \n");

        // 4. 读出hello world
        // if ((read(fd, line, MAXLINE)) > 0)
        // {
        //     printf("first child: %s\n", line);
        // }
        close(fd);

        // 5. 删除
        remove(PATHNAME);

        exit(0);
    }
    
    exit(0);
}

