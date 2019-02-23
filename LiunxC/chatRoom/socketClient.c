/*************************************************************************
	> File Name: socketClinet.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年02月23日 星期六 11时05分13秒
 ************************************************************************/

#include "common/socklib.h"
#define BUFSIZE 100

char IP[]="192.168.2.86";
int port=8731;

char name[]="银.TAMA";

int main(int arg,char*args[]){
    if(arg == 3){
        strcpy(IP,args[1]);
        port=strToInt(args[2]);
        strcpy(name,args[3]);
    }
    int sockfd = connect_to_server(IP,port);

    send(sockfd,name,sizeof(name),0);

    char buf[BUFSIZE];

    while(1){
        buf[0]='\0';
        scanf("%[^\n]",buf);getchar();
        send(sockfd,buf,strlen(buf),0);
    }

    close(sockfd);
    return 0;
}

