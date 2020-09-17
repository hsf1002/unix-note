#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

/*
    stat & chmod

echo hello > hi
hefeng@sw-hefeng:/home/work1/workplace/github/unix-note/04-文件和目录$ stat hi
  文件：'hi'
  大小：6         	块：8          IO 块：4096   普通文件
设备：802h/2050d	Inode：37095331    硬链接：1
权限：(0664/-rw-rw-r--)  Uid：( 1000/  hefeng)   Gid：( 1000/  hefeng)
最近访问：2020-09-17 10:30:13.000000000 +0800
最近更改：2020-09-17 10:42:01.781473474 +0800
最近改动：2020-09-17 10:42:01.781473474 +0800
创建时间：-

// 会有错误：hi futimens error
hefeng@sw-hefeng:/home/work1/workplace/github/unix-note/04-文件和目录$ ./a.out hi
hefeng@sw-hefeng:/home/work1/workplace/github/unix-note/04-文件和目录$ stat hi
  文件：'hi'
  大小：0         	块：0          IO 块：4096   普通空文件
设备：802h/2050d	Inode：37095331    硬链接：1
权限：(0664/-rw-rw-r--)  Uid：( 1000/  hefeng)   Gid：( 1000/  hefeng)
最近访问：2020-09-17 10:30:13.000000000 +0800
最近更改：2020-09-17 10:42:01.000000000 +0800
最近改动：2020-09-17 10:42:08.633557783 +0800
创建时间：-


*/
int main(int argc, char *argv[])
{
    struct stat status;
    struct timespec times[2];
    int i, fd;

    for (i=1; i<argc; ++i)
    {
        // 获取时间
        if (stat(argv[i], &status) < 0)
        {
            printf("%s stat error", argv[i]);
            continue;
        }

        // 保存时间
        times[0].tv_sec = status.st_atime;
        times[1].tv_sec = status.st_mtime;

        // 打开并截断里面内容
        if ((fd = open(argv[i], O_RDWR|O_TRUNC)) < 0)
        {
            printf("%s open error", argv[i]);
            continue;
        }

        // 重置时间
        if (futimens(fd, times) < 0)
        {
            printf("%s futimens error", argv[i]);
        }

        close(fd);
    }

    return(0);
}
