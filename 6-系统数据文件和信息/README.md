### 第六章  系统数据文件和信息

##### 口令文件

/etc/passwd  

用户名：加密口令：用户ID：组ID：注释字段：初始工作目录：初始shell

* 通常有一个用户名为root，用户ID为0的登录项（超级用户）
* 加密口令字段占位，内容存储在了另一个文件
* 某些字段可能为空，如果加密口令字段为空，意味着该用户没有口令
* shell字段包含一个可执行程序，用作登陆shell；为空，则取系统默认，如果是/dev/null，标明是一个设备，不是可执行文件，任何人无法以该用户登陆
* 为了阻止一个特定用户登陆，除了/dev/null外，还可以将/bin/false作为登陆shell，或/bin/true
* 使用nobody用户名的目的使任何人都可以登陆，只能访问人人皆可读写的文件
* 使用finger指令支持注释字段的附加信息

```
#include <pwd.h>
struct passwd *getpwnam(const char *name);
struct passwd *getpwuid(uid_t uid);
// 两个函数，若成功，返回指针，若出错，返回NULL

struct passwd {
                char *pw_name; /* user name */
                char *pw_passwd; /* user password */
                uid_t pw_uid; /* user id */
                gid_t pw_gid; /* group id */
                char *pw_gecos; /* real name */
                char *pw_dir; /* home directory */
                char *pw_shell; /* shell program */
              };
```

getpwuid由ls命令使用，getpwnam由login程序使用，只能查看登录名或用户ID，如果查看口令文件，需要：

```
#include <pwd.h>
struct passwd *getpwent(void);
// 若成功，返回指针，若出错或到达文件尾，返回NULL

void setpwent(void);
void endpwent(void);
```

* getpwent：返回口令文件中下一个记录项

* setpwent：将getpwent的读写地址指向密码文件开头
* endpwent：关闭这些文件

##### 阴影口令

/etc/shadow

加密口令是经单向加密算法处理过的用户口令副本，通常将加密口令存放在一个阴影口令的文件中，仅有几个用户ID为root的程序如login和passwd才可以访问

```
struct spwd 
{
    char *sp_namp; /* Login name */
    char *sp_pwdp; /* Encrypted password */
    long int sp_lstchg; /* Date of last change */
    long int sp_min; /* Minimum number of days between changes */
    long int sp_max; /* Maximum number of days between changes */
    long int sp_warn; /* Number of days to warn user to change the password */
    long int sp_inact; /* Number of days the account may be inactive */
    long int sp_expire; /* Number of days since 1970-01-01 until account expires */
    unsigned long int sp_flag; /* Reserved */
};
```

这组函数与口令文件的一组函数对应：

```
#include <shadow.h>
struct spwd *getspnam(const char *name);
struct spwd getspent(void);
// 两个函数，若成功，返回指针，若出错或到达文件尾，返回NULL

void setspent(void);
void endspent(void);
```

##### 组文件

/etc/group

查看组名或组ID：

```
#include<grp.h>
struct group *getgrpid(gid_t gid);
struct group *getgrnam(const char *name);
// 两个函数，若成功，返回指针，若出错，返回NULL
```

如果要搜索整个组文件：

```
#include<grp.h>
struct group *getgrent();
// 若成功，返回指针，若出错或到达文件尾，返回NULL
void setgrent();
void endgrent();
```

##### 附属组

使用附属组可以不必再显式的经常更改组，文件访问权限不仅检查组ID，还要检查附属组ID，一个用户可能参与多个项目，因此需要同时属于多个组

```
#include<unistd.h>
int getgroups(int gidsetsize, gid_t grouplist[]);  
// 若成功，返回附属组ID数，若出错，返回-1，将附加组id填写到grouplist中，最多gidsetsize个

#include<grp.h>
#include<unistd.h>/*非linux*/
int setgroups(int ngroups, const gid_t grouplist[]);   // 为进程设置附加组id表

#include<grp.h>   /*on linux and Solaris*/
#include<unistd.h>/*非linux 和 solaris*/
int initgroups(const char* username, gid_t basegid);
// 两个函数，若成功，返回0，若出错，返回-1
```

* getgroups：将进程所属用户的各附属组ID填写到数组中，如gidesetsize为0，返回附属组ID数
* setgroups：由超级用户调用为调用进程设置附属组ID表
* initgroups：只有超级用户才会调用initgroups，只有initgroups才调用setgroups

##### 其他数据文件

```
口令 /etc/passwd  <pwd.h>  passwd  getpwnam,getpwuid
组   /etc/group  <grp.h>  group  getgrnam,getgrgid
阴影 /etc/shadow  <shadow.h> spwd  getspnam
主机 /etc/hosts  <netdb.h> hostent  gethostbyname,gethostbyaddr
网络 /etc/network  <netdb.h> netend  getnetbyname,getnetbyaddr
协议 /etc/protocols  <netdb.h> protoent getprotobyname,getprotobynumber
服务 /etc/services  <netdb.h> servent  getservbyname,getservbyport
```

对于每个文件，至少有三个函数：

* get：读取下一条记录，如果需要，还会打开数据文件
* set：打开数据文件
* end：关闭数据文件

##### 登陆账户记录

utmp（who读取）文件记录当前登陆到系统的各个用户，wtmp（last读取）跟踪各个登陆和注销事件

##### 系统标识

uname：返回主机和操作系统相关信息

```
#include<sys/utsname.h>  
int uname(struct utsname *name);
// 若成功，返回非负值，若出错，返回-1
```

gethostname：只返回主机名，通常是TCP/IP网络上主机名

```
#include<unistd.h>        
int gethostname(char *name, int namelen);
// 若成功，返回0，若出错，返回-1
```

##### 时间和日期

**time：返回当前时间和日期，即日历时间**

```
include<time.h>       
time_t time(time_t *calptr);
// 若成功，返回时间值，若出错，返回-1

typedef long     time_t;    /* 时间值time_t 为长整型的别名*/
```

如果参数为非空，时间值也存放在calptr中

**clock_gettime：获取指定时钟的时间**

```
int clock_gettime(clockid_t clk_id, struct timespec *tp);
// 若成功，返回0，若出错，返回-1
int clock_settime(clockid_t clk_id, const struct timespec *tp);
// 若成功，返回0，若出错，返回-1

struct timespec {
    time_t tv_sec; 	// seconds 
    long tv_nsec; 	// and nanoseconds 
};
```

* CLOCK_REALTIME：系统实时时间，随系统实时时间改变而改变
* CLOCK_MONOTONIC：从系统启动这一刻起开始计时，不受系统时间被用户改变
* CLOCK_PROCESS_CPUTIME_ID：本进程到当前代码系统CPU花费的时间
* CLOCK_THREAD_CPUTIME_ID：本线程到当前代码系统CPU花费的时

**gettimeofday：已经弃用，有些程序还在使用，提供了比time更高的精度（微秒）**

```
#include<time.h>    
int gettimeofday(struct timeval *restrict tp, void *restrict tzp);
// 总是返回0，tzp的唯一合法值是NULL

struct timeval {
    long tv_sec;        /* seconds */
    long tv_usec;       /* microseconds */
};
```

##### gmtime和localtime：将日历时间转为分解的时间

秒可以超过59的理由是可以表示润秒

```
#include<time.h>
struct tm *gmtime(const time_t *calptr);  		
// 转化为国际标准时间的年、月、日、时、分、秒等，若出错，返回NULL
struct tm *localtime(const time_t *calptr);     
// 转化为本地时间(考虑到本地时区和夏令时标志)，若出错，返回NULL

struct tm {
   int tm_sec;    /* Seconds (0-60) */
   int tm_min;    /* Minutes (0-59) */
   int tm_hour;   /* Hours (0-23) */
   int tm_mday;   /* Day of the month (1-31) */
   int tm_mon;    /* Month (0-11) */
   int tm_year;   /* Year since 1900 */
   int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
   int tm_yday;   /* Day in the year (0-365, 1 Jan = 0) */
   int tm_isdst;  /* Daylight saving time */
};
```

##### mktime：本地时间转为time_t

```
#include<time.h>  
time_t mktime(struct tm *tmptr);
// 若成功，返回日历时间，若出错，返回-1
```

##### strftime：类似于printf的时间值函数，通过可用的多个参数定制产生的字符串

```
#include<time.h>     
size_t strftime(char *restrict buf, size_t maxsize, const char *restrict format, const struct tm *restrict tmptr);
size_t strftime(char *restrict buf, size_t maxsize, const char *restrict format, const struct tm *restrict tmptr, locale_t locale);
// 两个函数，若有空间，返回存入数组的字符数，否则，返回0

%a	Abbreviated weekday name *	Thu
%A	Full weekday name *	Thursday
%b	Abbreviated month name *	Aug
%B	Full month name *	August
%c	Date and time representation *	Thu Aug 23 14:55:02 2001
%C	Year divided by 100 and truncated to integer (00-99)	20
%d	Day of the month, zero-padded (01-31)	23
%D	Short MM/DD/YY date, equivalent to %m/%d/%y	08/23/01
%e	Day of the month, space-padded ( 1-31)	23
%F	Short YYYY-MM-DD date, equivalent to %Y-%m-%d	2001-08-23
%g	Week-based year, last two digits (00-99)	01
%G	Week-based year	2001
%h	Abbreviated month name * (same as %b)	Aug
%H	Hour in 24h format (00-23)	14
%I	Hour in 12h format (01-12)	02
%j	Day of the year (001-366)	235
%m	Month as a decimal number (01-12)	08
%M	Minute (00-59)	55
%n	New-line character ('\n')	
%p	AM or PM designation	PM
%r	12-hour clock time *	02:55:02 pm
%R	24-hour HH:MM time, equivalent to %H:%M	14:55
%S	Second (00-61)	02
%t	Horizontal-tab character ('\t')	
%T	ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S	14:55:02
%u	ISO 8601 weekday as number with Monday as 1 (1-7)	4
%U	Week number with the first Sunday as the first day of week one (00-53)	33
%V	ISO 8601 week number (00-53)	34
%w	Weekday as a decimal number with Sunday as 0 (0-6)	4
%W	Week number with the first Monday as the first day of week one (00-53)	34
%x	Date representation *	08/23/01
%X	Time representation *	14:55:02
%y	Year, last two digits (00-99)	01
%Y	Year	2001
%z	ISO 8601 offset from UTC in timezone (1 minute=1, 1 hour=100)
If timezone cannot be termined, no characters	+100
%Z	Timezone name or abbreviation *
If timezone cannot be termined, no characters	CDT
%%	A % sign	%
```

##### strptime：是strftime反过来的版本

```
字符串                                   格式化字符串
    \                                       /
  strptime                             strftime
          \                             /
                struct_tm(分解时间)
                 gmtime |localtime
                        |
                        |mktime
          tv_sec                       tv_sec
timeval------------>time_t(日历时间)<------------timespec
   \                    |                       /                    
      \                 |                   /                     
          \             | time         /
 gettimeofday \         |          / clock_time                        
                       内核
```

localtime、mktime和strftime受到环境变量TZ时区的影响，如果定义了TZ则替换系统默认，如果TZ为空，则使用UTC
