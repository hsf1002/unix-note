#include <stdio.h>
#include <stdlib.h>


static void my_exit1(void);
static void my_exit2(void);

/*
    atexit: 退出注册函数 

main is done 
first exit handler 
first exit handler 
second exit handler

*/
int main(int argc, char *argv[])
{
    if (atexit(my_exit2) != 0)
        ferror("register my_exit2 failed ");
    if (atexit(my_exit1) != 0)
        ferror("register my_exit1 failed ");
    if (atexit(my_exit1) != 0)
        ferror("register my_exit1 failed ");
    
    printf("main is done \n");

    return(0);
}

static void my_exit1(void)
{
    printf("first exit handler \n");
}

static void my_exit2(void)
{
    printf("second exit handler \n");
}
