#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <pwd.h>


struct passwd * getpwnam(const char *name);

/*
    获取口令文件信息     
    cannot work in macOS


hefeng@sw-hefeng:/home/work1/workplace/github/unix-note/06-系统数据文件和信息$ ./a.out root
get the info 
name = root, pw = x, uid = 0, gid = 0, shell = /bin/bash, home = /root 
hefeng@sw-hefeng:/home/work1/workplace/github/unix-note/06-系统数据文件和信息$ ./a.out hefeng
continue. ptr->pw_name = root 
continue. ptr->pw_name = daemon 
continue. ptr->pw_name = bin 
continue. ptr->pw_name = sys 
continue. ptr->pw_name = sync 
continue. ptr->pw_name = games 
continue. ptr->pw_name = man 
continue. ptr->pw_name = lp 
continue. ptr->pw_name = mail 
continue. ptr->pw_name = news 
continue. ptr->pw_name = uucp 
continue. ptr->pw_name = proxy 
continue. ptr->pw_name = www-data 
continue. ptr->pw_name = backup 
continue. ptr->pw_name = list 
continue. ptr->pw_name = irc 
continue. ptr->pw_name = gnats 
continue. ptr->pw_name = nobody 
continue. ptr->pw_name = systemd-timesync 
continue. ptr->pw_name = systemd-network 
continue. ptr->pw_name = systemd-resolve 
continue. ptr->pw_name = systemd-bus-proxy 
continue. ptr->pw_name = syslog 
continue. ptr->pw_name = _apt 
continue. ptr->pw_name = messagebus 
continue. ptr->pw_name = uuidd 
continue. ptr->pw_name = lightdm 
continue. ptr->pw_name = whoopsie 
continue. ptr->pw_name = avahi-autoipd 
continue. ptr->pw_name = avahi 
continue. ptr->pw_name = dnsmasq 
continue. ptr->pw_name = colord 
continue. ptr->pw_name = speech-dispatcher 
continue. ptr->pw_name = hplip 
continue. ptr->pw_name = kernoops 
continue. ptr->pw_name = pulse 
continue. ptr->pw_name = rtkit 
continue. ptr->pw_name = saned 
continue. ptr->pw_name = usbmux 
get the info 
name = hefeng, pw = x, uid = 1000, gid = 1000, shell = /bin/bash, home = /home/hefeng 

*/
int main(int argc, char *argv[])
{
    struct passwd *ptr = getpwnam(argv[1]);
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
    }

    // 关闭打开的文件
    endpwent();
    
	return ptr;
}
