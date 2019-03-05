/*************************************************************************
	> File Name: common.h
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月05日 星期二 18时44分51秒
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>    
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>

// 向PiHealthLog 文件中写入内容 返回成功print的char的个数
// TODO 加文件锁
int write_Pi_log(char *PiHealthLog,const char*format,...);

#endif
