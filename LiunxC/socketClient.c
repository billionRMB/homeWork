/*************************************************************************
	> File Name: socketClient.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年02月21日 星期四 16时57分34秒
 ************************************************************************/
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

#define BUFSIZE 100

char IP[]="192.168.1.40";
int port=8731;

char name[]="银.TAMA";

int main(){
    int sockfd;
    char buf[BUFSIZE];
    struct sockaddr_in their_add;
    while((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1);
    printf("get sockfd\n");

    their_add.sin_family = AF_INET;
    their_add.sin_port = htons(port);
    their_add.sin_addr.s_addr=inet_addr(IP);
    bzero(&(their_add.sin_zero),8);

    while(connect(sockfd,(struct sockaddr*)&their_add,sizeof(struct sockaddr)) == -1);
    printf("Get the Server~Cheers!\n");

    send(sockfd,name,sizeof(name),0);
    while(1){
        scanf("%s",buf);
        send(sockfd,buf,strlen(buf),0);
    }
    close(sockfd);
    return 0;
}
