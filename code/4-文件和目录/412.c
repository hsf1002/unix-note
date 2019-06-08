#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/*
    stat & chmod

before
-rw-rw-rw-  1 sky  staff     0  6  9 06:33 foo
-rw-------  1 sky  staff     0  6  9 06:33 bar

after
-rw-rwSrw-  1 sky  staff     0  6  9 06:33 foo
-rw-r--r--  1 sky  staff     0  6  9 06:33 bar

*/
int main(int argc, char *argv[])
{
    struct stat status;

    // 获取当前权限
    if (stat("foo", &status) < 0)
        ferror("stat error for foo");

    // 设置权限，打开设置组ID，关闭组执行位
    if (chmod("foo", (status.st_mode & ~S_IXGRP) | S_ISGID) < 0)
        ferror("chmod error for foo");

    // 设置绝对位：RW-R--R--
    if (chmod("bar", S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) < 0)
        ferror("chmod error for bar");

    return(0);
}
