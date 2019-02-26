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

#define DEBUG

#if defined DEBUG
#define DBG(s){printf("DBG:%s = %d\n",#s,s);}
#define DBS(s){printf("DBG:%s = %s\n",#s,s);}
#else
#define DBG(s){}
#define DBS(S){}
#endif
struct Message{
 char from[20];
 int flag;//若flag为1则为私聊信息，0为公聊信息，2则为服务器的通知信息
 char message[1024];
};
typedef struct clientInfo{
    struct sockaddr_in saddr_client;
    int clength;
}clientInfo;
typedef struct serverCFuntion{
    char * tips;
    clientInfo* cInfo;
    int(*process_request)(int,clientInfo*);
}SCFL;
// 初始化一下
void initSCFL(SCFL** serverCF);
// 设置server 启动时的话语
void setTips(SCFL* sf,char*tips);

// 获得一个server的socket
int make_server_socket(int,int);
// 连接到服务器
int connect_to_server(char*,int);
// string to int 
int strToInt(char*);
//　从pathname文件下读key - value
int get_conf_value(char *pathname, char *key_name,char** value);
// 抽象的一个server的controller
int runServer(SCFL* serverCF,int);
// 封装一下send 函数来发送一个Struct message
int sendMessage(int sockfd,Message*msg);
#endif

