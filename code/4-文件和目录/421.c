#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

/*
    stat & chmod

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
        time[0] = status.st_atime;
        time[1] = status.st_mtime;

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
