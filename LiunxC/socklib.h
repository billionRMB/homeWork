/*************************************************************************
	> File Name: socklib.h
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年02月22日 星期五 16时52分43秒
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
int make_server_socket(int,int);
int connect_to_server(char*,int);
int strToInt(char*);

int strToInt(char*str){
    int ans = 0;
    for(int i = 0;str[i];i++){
        ans = ans*10 + str[i] - '0';
    }
    return ans;
}

int connect_to_server(char* IP,int port){
    int sockfd;
    struct sockaddr_in their_add;
    while((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1);

    their_add.sin_family = AF_INET;
    their_add.sin_port = htons(port);
    their_add.sin_addr.s_addr=inet_addr(IP);
    bzero(&(their_add.sin_zero),8);

    while(connect(sockfd,(struct sockaddr*)&their_add,sizeof(struct sockaddr)) == -1);
    return sockfd;

}

int make_server_socket(int port,int q_size){
    int socket_id;
    // 初始化 socket
    if((socket_id = socket(AF_INET,SOCK_STREAM,0)) == -1){
        perror("create socket:");
        exit(1);
    }

    // 初始化地址端口
    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socket_id,(struct sockaddr*)&saddr,sizeof(saddr)) == -1){
        perror("bind socket error:");
        //注意关闭一下
        close(socket_id);
        exit(1);
    }

    if( listen(socket_id,q_size) == -1 ){
        perror("listen socket error:");
        //注意关闭一下
        close(socket_id);
        exit(1);
    }

    return socket_id;
}
#endif
