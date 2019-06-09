#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <pwd.h>


struct passwd * getpwnam(const char *name);

/*
    获取口令文件信息     
    cannot work in macOS
*/
int main(int argc, char *argv[])
{
    struct passwd *ptr = getpwnam("root");
    if (NULL == ptr)
    {
        printf("passwd is null \n");
        return -1;
    }
    printf("name = %s, pw = %s, uid = %ld, gid = %ld, shell = %s, home = %s \n", ptr->pw_name, ptr->pw_passwd, \
        ptr->pw_uid, ptr->pw_gid, ptr->pw_shell, ptr->pw_dir);

    return(0);
}


struct passwd * getpwnam(const char *name)
{
    struct passwd *ptr = NULL;

    // 将getpwent的读写地址指向密码文件开头
    setpwent();

    // 返回口令文件中的下一个记录项
    while ((ptr = getpwent()) != NULL)
    {
        if (strcmp(name, ptr->pw_name) == 0)
        {
            printf("get the info \n");
            break;
        }

        printf("continue. ptr->pw_name = %s \n", ptr->pw_name);
        ptr++;
        // 关闭打开的文件
        endpwent();
    }

    return ptr;
}
