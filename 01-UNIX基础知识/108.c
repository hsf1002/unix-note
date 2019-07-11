#include <stdio.h>
#include <err.h>
#include <errno.h>

/*
    ./a.out 
    EACCES: Permission denied 
    ./a.out: No such file or directory

    #include <stdio.h>
    void perror(const char *msg);   
    // 基于errno的当前值，在标准出错上产生一条出错信息，然后返回

    #include <string.h>
    char * strerror(int errnum);
    // 将errnum（通常就是errno值）映射为一个出错信息字符串，并返回此字符串的指针
*/
int main(int argc, char *argv[])
{
    fprintf(stderr, "EACCES: %s \n", strerror(EACCES));

    errno = ENOENT;

    perror(argv[0]);

    exit(0);
}
