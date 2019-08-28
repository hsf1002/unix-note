#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <aio.h>

#define BSZ 4096
#define NBUF 8

enum rwop
{
    UNUSED = 0,
    READ_PENDING = 1,
    WRITE_PEPNDING = 2,
};

struct buf
{
    enum rwop op;
    int last;
    struct aiocb aiocb;
    unsigned char data[BSZ];
};

// 带有8个缓冲区的bufs
struct buf bufs[NBUF];

unsigned char 
translate(unsigned char c)
{
    if (isalpha(c))
    {
        if (c >= 'n')
            c -= 13;
        else if (c >= 'a')
            c += 13;
        else if (c >= 'N')
            c -= 13;
        else
            c += 13;
    }

    return c;
}

/*
    使用异步IO实现ROT-13算法
*/
int 
main(int argc, char *argv[]) 
{
    int ifd, ofd, i, j, err, numop, n;
    struct stat sbuf;
    const struct aiocb *aiolist[NBUF];
    off_t off = 0;

    if (argc != 3)
    {
        fprintf(stderr, "usage: rot-13 infile outfile \n");
        exit(1);
    }

    // 打开输入文件
    if ((ifd = open(argv[1], O_RDONLY)) < 0)
    {
        perror("open error \n");
    }

    // 创建输出文件
    if ((ofd = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0)
    {
        perror("create error \n");
    }
    printf("ofd 1 = %d \n", ofd);

    // 获取文件属性-大小
    if (fstat(ifd, &sbuf) < 0)
        perror("fstat error \n");

    // 初始化缓冲区
    for (i=0; i<NBUF; i++)
    {
        bufs[i].op = UNUSED;
        bufs[i].aiocb.aio_buf = bufs[i].data;
        bufs[i].aiocb.aio_sigevent.sigev_notify = SIGEV_NONE;
        aiolist[i] = NULL;
    }

    numop = 0;

    for (;;)
    {
        for (i=0; i<NBUF; i++)
        {
            switch (bufs[i].op)
            {
                // 从输入文件读取数据
                case UNUSED:
                {
                    if (off < sbuf.st_size)
                    {
                        bufs[i].op = READ_PENDING;
                        bufs[i].aiocb.aio_fildes = ifd;
                        bufs[i].aiocb.aio_offset = off;
                        off += BSZ;

                        if (off >= sbuf.st_size)
                        {
                            bufs[i].last = 1;
                        }
                        bufs[i].aiocb.aio_nbytes = BSZ;
                        if (aio_read(&bufs[i].aiocb) < 0)
                        {
                            perror("aio_read error \n");
                        }
                        aiolist[i] = &bufs[i].aiocb;
                        numop++;
                    }
                }
                break;
                // 读操作完成，翻译并写入
                case READ_PENDING:
                {
                    if ((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS)
                        continue;
                    if (err != 0)
                    {
                        if (err == -1)
                            perror("aio_error failed \n");
                        else 
                            perror("read failed \n");
                    }

                    if ((n = aio_return(&bufs[i].aiocb)) < 0)
                        perror("aio_return failed \n");
                    if (n != BSZ && !bufs[i].last)
                        fprintf(stderr, "short read (%d/%d) \n", n, BSZ);
                    
                    for (j=0; j<n; j++)
                        bufs[i].data[j] = translate(bufs[i].data[j]);
                    
                    bufs[i].op = WRITE_PEPNDING;
                    bufs[i].aiocb.aio_fildes = ofd;
                    bufs[i].aiocb.aio_nbytes = n;

                    if (aio_write(&bufs[i].aiocb) < 0)
                        perror("aio_write failed \n");
                }
                break;
                // 写操作已完成，标记buf为unused
                case WRITE_PEPNDING:
                {
                    if ((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS)
                        continue;
                    if (err != 0)
                    {
                        if (err == -1)
                            perror("aio_error failed \n");
                        else 
                            perror("write failed \n");
                    }

                    if ((n = aio_return(&bufs[i].aiocb)) < 0)
                        perror("aio_return failed \n");
                    if (n != bufs[i].aiocb.aio_nbytes)
                        fprintf(stderr, "short write (%d/%d) \n", n, BSZ);
                    
                    aiolist[i] = NULL;
                    bufs[i].op = UNUSED;
                    numop--;
                }
                break;
            
            default:
                break;
            }
        }

        if (numop == 0)
        {
            if (off >= sbuf.st_size)
                break;
        }
        else
        {
            if (aio_suspend(aiolist, NBUF, NULL) < 0)
                perror("aio_suspend failed \n");
        }
    }

    bufs[0].aiocb.aio_fildes = ofd;
    printf("ofd 2 = %d \n", ofd);

    if (aio_fsync(O_SYNC, &bufs[0].aiocb) < 0)
        perror("aio_fsync failed \n");
    
    exit(0);
}


