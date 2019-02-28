/*************************************************************************
	> File Name: 2.socketServer.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年02月23日 星期六 18时46分52秒
 ************************************************************************/

#include "common/socklib.h"
int process_request(int connect_fd,clientInfo* s){
    int pid = fork();
    switch(pid){
        case -1:return -1;
        case 0:{
            char name[20];
            int nameL = recv(connect_fd,name,100,0);
            name[nameL] = '\0';
            printf("%s Login in from %s\n",name,inet_ntoa(s->saddr_client.sin_addr));
            int n;
            /*
            // 这个是用来接收mesage struct 的。。
            message msg;
            while(recvMessage(connect_fd,&msg)!=-1){
                printf("msg:recv:%s\n",msg.message);
            }
            */
            char buff[100];
            while((n = recv(connect_fd,buff,100,0))> 0){
                buff[n] = '\0';
                printf("%s : %s\n",name, buff);
            }
            close(connect_fd);
            printf("%s close\n",name);
            exit(0);
        }break;
    }
}

int main(int args,char* argc[]){
    int port = -1;
    if(args == 2){
        port = strToInt(argc[1]);
    }
    SCFL* serverCF = NULL;
    char Tips[] = "Server Start  ..\n";
    initSCFL(&serverCF);
    setTips(serverCF,Tips);
    // 绑定server端执行的函数
    serverCF->process_request = process_request;
    // 运行server
    runServer(serverCF,port);
    return 0;
}
