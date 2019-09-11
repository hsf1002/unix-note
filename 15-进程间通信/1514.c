#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
//#include <sys/wait.h>
#include <ctype.h>

int
main(void)
{
    int c;
    while ((c = getchar()) != EOF)
    {
        if (isupper(c))
            c = tolower(c);
        
        if (putchar(c) == EOF)
            perror("putchar error");
        
        if (c == '\n')
            fflush(stdout);
    }

    exit(0);
}

