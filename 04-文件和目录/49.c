#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define RWRWRW (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

/*
    umask

umask 
0022
skydeiMac:4-文件和目录 sky$ ./a.out 
skydeiMac:4-文件和目录 sky$ ll foo bar 
-rw-rw-rw-  1 sky  staff  0  6  8 18:15 foo  // 先将umask清空为0，不屏蔽任何权限
-rw-------  1 sky  staff  0  6  8 18:15 bar  // 屏蔽所有组和其他用户的访问权限

*/
int main(int argc, char *argv[])
{
    umask(0);

    if (creat("foo", RWRWRW) < 0)
        printf("create error for foo \n");
    
    umask(S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);

    if (creat("bar", RWRWRW) < 0)
        printf("create error for bar \n");

    return(0);
}
