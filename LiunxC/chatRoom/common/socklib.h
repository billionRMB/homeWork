/*************************************************************************
	> File Name: socklib.h
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年02月23日 星期六 11时01分24秒
 ************************************************************************/
#ifndef _SOCKLIB_H
#define _SOCKLIB_H
#include<stdio.h>
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

#if defined DEBUG
#define DBG(s){printf("DBG:%s = %d\n",#s,s);}
#else
#define DBG(s){}
#endif

int make_server_socket(int,int);
int connect_to_server(char*,int);
int strToInt(char*);
int get_conf_value(char *pathname, char *key_name,char* value);

#endif

