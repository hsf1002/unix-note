#include <stdio.h>
#include <sys/stat.h>
#include <sys/errno.h>

#define MAXLINE 100


void make_temp(char *pathname);

/*
    mkdtemp和mkstemp

./a.out
trying to create the first temp file...
temp name = /tmp/dirZ88tHx
file existed
trying to create the second temp file...
Bus error: 10
*/
int main(int argc, char *argv[])
{
    char good_template[] = "/tmp/dirXXXXXX";    // the right way
    char *wrong_template = "/tmp/dirXXXXXX";    // the wrong way
   
    printf("trying to create the first temp file... \n");
    make_temp(good_template);

    printf("trying to create the second temp file... \n");
    make_temp(wrong_template);

    return(0);
}

void make_temp(char *pathname)
{
    int fd;
    struct stat status;

    if ((fd = mkstemp(pathname)) < 0)
    {
        ferror("cannot create the temp file");
    }

    printf("temp name = %s \n", pathname);
    close(fd);

    if (stat(pathname, &status) < 0)
    {
        if (errno == ENOENT)
        {
            printf("file doesnot exist");
        }
        else
        {
            printf("stat failed");
        }
    }
    else
    {
        printf("file existed \n");
        unlink(pathname);
    }
}
