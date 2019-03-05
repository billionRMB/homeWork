/*************************************************************************
	> File Name: common.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月05日 星期二 18时45分53秒
 ************************************************************************/

#include"common.h"

// 向PiHealthLog 文件中写入内容，返回printed 的char的个数
// TODO 加文件锁
int write_Pi_log(char* PiHealthLog,const char*format,...){
    // 加入a / a+ 的区别
    FILE* fd = fopen(PiHealthLog,"a");
    time_t timep;struct tm *p;
    time(&timep);
    p=localtime(&timep);
    // 打印时间
    int n = fprintf(fd,"\033[47;30m[%04d-%02d-%02d %02d:%02d]\033[0m ",p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min);
    va_list args;va_start(args,format);
    // fprintf 的变形可以传入va_list
    n += vfprintf(fd,format,args);
    // 刷新一下
    fflush(fd);
    va_end(args);fclose(fd);

    return n;
}
