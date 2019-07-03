#include <stdio.h>

#define MAXLINE 100

/*
    fgets/fputs
*/
int main(int argc, char *argv[])
{
    char buf[MAXLINE];

    while (fgets(buf, MAXLINE, stdin) != NULL)
    {
        if (fputs(buf, stdout) == EOF)
        {
            printf("fputs error");
        }
    }

    if (ferror(stdin))
    {
        printf("fgets error");
    }

    return(0);
}
