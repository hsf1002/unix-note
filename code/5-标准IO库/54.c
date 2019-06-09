#include <stdio.h>

/*
    getc/putc

*/
int main(int argc, char *argv[])
{
    int c;

    while ((c = getc(stdin)) != EOF)
    {
        if (putc(c, stdout) == EOF)
        {
            printf("putc error");
        }
    }

    if (ferror(stdin))
    {
        printf("getc error");
    }

    return(0);
}
