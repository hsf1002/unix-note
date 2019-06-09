#include <stdio.h>

#define MAXLINE 100


void pr_stdio(const char * , FILE *);
int is_unbufferred(FILE *);
int is_linebufferred(FILE *);
int buffer_size(FILE *);

/*
    为三个标准流以及一个与普通文件相关的流打印有关缓冲的状态信息

skydeiMac:5-标准IO库 sky$ ./a.out 
enter any character 
a
one line to standard error 
stream = stdin, line bufferred, buffer size = 4096 
stream = stdout, line bufferred, buffer size = 4096 
stream = stderr, unbufferred, buffer size = 0 
stream = /etc/passwd, fully bufferred, buffer size = 4096 
skydeiMac:5-标准IO库 sky$ ./a.out < /etc/group > std.out 2> std.err
skydeiMac:5-标准IO库 sky$ cat std.err 
skydeiMac:5-标准IO库 sky$ cat std.out
enter any character 
one line to standard error 
stream = stdin, fully bufferred, buffer size = 4096 
stream = stdout, fully bufferred, buffer size = 4096 
stream = stderr, unbufferred, buffer size = 0 
stream = /etc/passwd, fully bufferred, buffer size = 4096 

1. stdin, stdout,系统默认行缓冲，stderr系统默认不缓冲，普通文件则全缓冲
2. 将stdin，stdout都重定向到普通文件，再次运行，则变成了全缓冲，而stderr依然是不缓冲
*/
int main(int argc, char *argv[])
{
    FILE *fp;

    fputs("enter any character \n", stdout);

    if (getchar() == EOF)
    {
        ferror("getchar error");
    }

    fputs("one line to standard error \n", stdout);

    pr_stdio("stdin", stdin);
    pr_stdio("stdout", stdout);
    pr_stdio("stderr", stderr);

    if ((fp = fopen("/etc/passwd", "r")) == NULL)
    {
        ferror("fopen error");
    }

    if (getc(fp) == EOF)
    {
        ferror("getchar error");
    }

    pr_stdio("/etc/passwd", fp);
    
    return(0);
}

void pr_stdio(const char *name, FILE *fp)
{
    printf("stream = %s, ", name);

    if (is_unbufferred(fp))
    {
        printf("unbufferred");
    }
    else if (is_linebufferred(fp))
    {
        printf("line bufferred");
    }
    else
    {
        printf("fully bufferred");
    }

    printf(", buffer size = %d \n", buffer_size(fp));
}

#if defined(_IO_UNBUFFERED)
int is_unbufferred(FILE *fp)
{
    return (fp->_flags & _IO_UNBFFERED);
}

int is_linebufferred(FILE *fp)
{
    return (fp->_flags & _IO_LINE_BUFF);
}

int buffer_size(FILE *fp)
{
    return (fp->_IO_buf_end - fp->_IO_buf_base);
}
#elif defined(__SNBF)
int is_unbufferred(FILE *fp)
{
    return (fp->_flags & __SNBF);
}

int is_linebufferred(FILE *fp)
{
    return (fp->_flags & __SLBF);
}

int buffer_size(FILE *fp)
{
    return (fp->_bf._size);
}
#elif defined(_IONBF)
#ifdef _LP64
#define _flag __pad[4]
#define _ptr __pad[1]
#define _base __pad[2]
#endif

int is_unbufferred(FILE *fp)
{
    return (fp->_flag & _IONBF);
}

int is_linebufferred(FILE *fp)
{
    return (fp->_flag & _IOLBF);
}

int buffer_size(FILE *fp)
{
#ifdef _LP64
    return (fp->base - fp->_ptr);
#else
    return (BUFSIZ);
#endif   
}
#else
#error unknown stdio implementation
#endif
