#include <stdio.h>
#include <dirent.h>

/*

./a.out .
.
..
.DS_Store
103.c
104.c
105.c
107.c
108.c
110.c
a.out
README.md

    打印指定目录下所有文件和目录
*/
int main(int argc, char **argv)
{
    DIR *dp;
    struct dirent *dirp;

    if (argc != 2)
    {
        printf("usage: must be a directory! \n");
        return 1;
    }
    
    if ((dp = opendir(argv[1])) == NULL)
    {
        printf("cannot open %s \n", argv[1]);
        return 1;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        printf("%s\n", dirp->d_name);
    }

    closedir(dp);

    return 0;
}
