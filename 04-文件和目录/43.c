#include <stdio.h>
#include <sys/stat.h>


/*
    检查文件类型

./a.out /etc/passwd /etc /dev /dev/lockstat /dev/tty
/etc/passwd: regular
/etc: symbolic link
/dev: directory
/dev/lockstat: charactor special
/dev/tty: charactor special

*/
int main(int argc, char *argv[])
{
    int i;
    struct stat buf;
    char *ptr;

    for (i=1; i<argc; ++i)
    {
        printf("%s: ", argv[i]);

        if (lstat(argv[i], &buf) < 0)
        {
            printf("lstat error, continue \n");
            continue;
        }

        if (S_ISREG(buf.st_mode))
        {
            ptr = "regular";
        }
        else if (S_ISDIR(buf.st_mode))
        {
            ptr = "directory";
        }
        else if (S_ISCHR(buf.st_mode))
        {
            ptr = "charactor special";
        }
        else if (S_ISBLK(buf.st_mode))
        {
            ptr = "block spectial";
        }
        else if (S_ISFIFO(buf.st_mode))
        {
            ptr = "fifo";
        }
        else if (S_ISLNK(buf.st_mode))
        {
            ptr = "symbolic link";
        }
        else if (S_ISSOCK(buf.st_mode))
        {
            ptr = "socket";
        }
        else
        {
            ptr = "unknown mode";
        }

        printf("%s\n", ptr);
    }

    exit(0);
}
