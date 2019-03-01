/*************************************************************************
	> File Name: socketServer.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年02月23日 星期六 11时06分35秒
 ************************************************************************/

#include "common/socklib.h"
#define MAXLINE 4096  
int port = 8731;

void process_request(int connect_fd,struct sockaddr_in* s){
    int pid = fork();
    switch(pid){
        case -1:return;
        case 0:{
            char name[20];
            int nameL = recv(connect_fd,name,MAXLINE,0);
            name[nameL] = '\0';
            printf("%s Login in from %s\n",name,inet_ntoa(s->sin_addr));
            int n;
            char buff[100];
            while((n = recv(connect_fd,buff,MAXLINE,0))> 0){
                buff[n] = '\0';  
                printf("%s : %s\n",name, buff);  
            }
            close(connect_fd);  
            printf("%s close\n",name);
            exit(0);
        }break;
    }
}

int main(int argc, char** argv){  
    if(argc > 1){
        port = strToInt(argv[1]);
    }

    char filePath[]="/home/gintama/homeWork/LiunxC/chatRoom/common/chatroom.conf";

    char name[10];
    get_conf_value(filePath,"ip",name);

    // 获得服务器的socket
    int socket_fd = make_server_socket(port,10);
    int connect_fd;
    printf("======waiting for client's request======\n");  
    struct sockaddr_in saddr_client;
    int clengh = sizeof(saddr_client);
    while(1){  
        //阻塞直到有客户端连接，不然多浪费CPU资源。  
        if( (connect_fd = accept(socket_fd, (struct sockaddr*)&saddr_client,(socklen_t*)&clengh)) == -1){  
        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);  
        exit(0);
        }  
        process_request(connect_fd,&saddr_client);
    }  
    close(socket_fd);  
}  
