/*************************************************************************
	> File Name: socketClinet.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年02月23日 星期六 11时05分13秒
 ************************************************************************/

#include "common/socklib.h"

char filePath[] = "./common/chatroom.conf";

char* IP = NULL;
char* port = NULL;
char* name = NULL;
char* cport = NULL;
char* log_file = NULL;


int process_request(int connect_fd){
    int pid = fork();
    switch(pid){
        case -1:return -1;
        case 0:{
            FILE* fd;
            fd = fopen(log_file,"a");
            if(fd == NULL){
                printf("Wrong open %s\n",log_file);
                exit(-1);
            }
            message msg;
            while(recvMessage(connect_fd,&msg)!=-1){
                printf("msg.recv:%s\n",msg.message);
                fprintf(fd,"from:%s:flag:%d:message:%s\n",msg.from,msg.flag,msg.message);
            }
            close(connect_fd);
            exit(0);
        }break;
    }
}


// 初始化 配置变量
void init(){
    get_conf_value(filePath,"server_ip",&IP);
    get_conf_value(filePath,"server_port",&port);
    get_conf_value(filePath,"my_name",&name);
    get_conf_value(filePath,"clinet_port",&cport);
    get_conf_value(filePath,"log_file",&log_file);
}

int main(){
    // 初始化 配置变量
    init();
    // 获得套接字
    DBS(port);
    int sockfd = connect_to_server(IP,strToInt(port));
    // 发送姓名
    send(sockfd,name,sizeof(name),0);

    char buf[100];

    int pid = fork();
    
    if(pid == -1){
        printf("Wrong fork\n");
        exit(-1);
    }
    // 子进程
    if(pid == 0){
        process_request(sockfd);
    }else{
    // 父进程
        while(1){
            buf[0]='\0';
            scanf("%[^\n]",buf);getchar();
            send(sockfd,buf,strlen(buf),0);
            /*
            // 这个是用来发送struct message
            message msg;
            sendMessage(sockfd,&msg);
            strcpy(msg.message,buf);
            */
        }
        wait(NULL);
    }

    close(sockfd);
    return 0;
}

