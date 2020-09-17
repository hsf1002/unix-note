#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>



int major(dev_t dev)
{
	return (int)(dev);
}

int minor(dev_t dev)
{
	return (int)(dev);
}

/*
    st_dev&st_rdev

*/
int main(int argc, char *argv[])
{
    struct stat status;
    int i;

    for (i=1; i<argc; ++i)
    {
        printf("%s: ", argv[i]);

        if (stat(argv[i], &status) < 0)
        {
            printf("stat error");
            continue;
        }

        printf("dev = %d%d", major(status.st_dev), minor(status.st_dev));

        // 只有字符特殊文件或块特殊文件才有st_rdev，即实际的设备号
        if (S_ISCHR(status.st_mode) || S_ISBLK(status.st_mode))
        {
            printf(" (%s) rdev = %d%d", (S_ISCHR(status.st_mode)) ? "character" : "block", 
                major(status.st_rdev), minor(status.st_rdev));
        }

        printf("\n");
    }

    return(0);
}
