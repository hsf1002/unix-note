#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

// 限制复制内容的大小为1G
#define COPYINCR (1024 * 1024 * 1024)

/*
    使用存储映射IO复制文件
*/
int 
main(int argc, char *argv[]) 
{
    int ifd, ofd;
    void *src, *dst;
    size_t copysz;
    struct stat sbuf;
    off_t fsz = 0;

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <fromfile> <tofile> \n", argv[0]);
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

    // 取文件大小
    if (fstat(ifd, &sbuf) < 0)
    {
        perror("fstat error \n");
    }          
    
    // 设置文件大小
    if (ftruncate(ofd, sbuf.st_size) < 0)
    {
        perror("ftruncate error \n");
    }

    while (fsz < sbuf.st_size)
    {
        if ((sbuf.st_size - fsz) > COPYINCR)
            copysz = COPYINCR;
        else
            copysz = sbuf.st_size - fsz;
        // 将输入文件映射到内存，只读
        if ((src = mmap(0, copysz, PROT_READ, MAP_SHARED, ifd, fsz)) == MAP_FAILED)
        {
            perror("mmap error for input \n");
        }
        // 将输出文件映射到内存，读写
        if ((dst = mmap(0, copysz, PROT_READ|PROT_WRITE, MAP_SHARED, ofd, fsz)) == MAP_FAILED)
        {
            perror("mmap error for output \n");
        }

        // 将输入缓冲区的内容复制到输出缓冲区，从输入缓冲区读直接从文件中读，向输出缓冲区写直接写到文件
        memcpy(dst, src, copysz);
        // 解除映射
        munmap(src, copysz);
        // 解除映射
        munmap(dst, copysz);
        fsz += copysz;
    }
    
    exit(0);
}


