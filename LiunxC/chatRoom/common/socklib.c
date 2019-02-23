/*************************************************************************
	> File Name: socklib.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年02月23日 星期六 11时02分06秒
 ************************************************************************/
#include "socklib.h"
int strToInt(char*str){
    int ans = 0;
    for(int i = 0;str[i];i++){
        ans = ans*10 + str[i] - '0';
    }
    return ans;
}

int connect_to_server(char* IP,int port){
    int sockfd;
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("make socket:");
        exit(1);
    }
    struct sockaddr_in their_add;
    their_add.sin_family = AF_INET;
    their_add.sin_port = htons(port);
    their_add.sin_addr.s_addr=inet_addr(IP);
    bzero(&(their_add.sin_zero),8);

    if(connect(sockfd,(struct sockaddr*)&their_add,sizeof(struct sockaddr)) == -1){
       perror("connect server failed:");  
       close(sockfd);
       exit(1);
    }
    return sockfd;

}

int make_server_socket(int port,int q_size){
    int socket_id;
    // 初始化 socket
    if((socket_id = socket(AF_INET,SOCK_STREAM,0)) == -1){
        printf("create socket error: %s\n",strerror(errno));
        exit(1);
    }
    // 初始化地址端口
    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socket_id,(struct sockaddr*)&saddr,sizeof(saddr)) == -1){
        printf("bind socket error:%s\n",strerror(errno));
        close(socket_id);
        exit(1);
    }

    if( listen(socket_id,q_size) == -1 ){
        printf("listen socket error:%s\n",strerror(errno));
        close(socket_id);
        exit(1);
    }
    return socket_id;
}
// 获得从这个buff开始到结束的字符的字符串
char* get_a(char* buff,char endChar){
    int i = 0;
    while(buff[i]!=endChar)i++;
    char* result = calloc(i,sizeof(char));
    strncpy(result,buff,i);
    return result;
}

int get_conf_value(char *pathname, char *key_name,char* value){
    int fd;
    if((fd = open(pathname,O_RDONLY)) == -1){
        printf("Wrong open %s\n",pathname);
        return -1;
    }
    // 获取文件长度
    int length = lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);

    char* buff = calloc(length,sizeof(char));

    if(read(fd,buff,length) == -1){
        printf("Failed to read\n");
        return -1;
    }

    int len = strlen(key_name),i = 0;

    while(i < length){
        char* temp = get_a(buff+i,'=');
        if(strncmp(key_name,temp,len) == 0){
            strcpy(value,get_a(buff+i+len+1,'\n'));
            break;
        }else{
            i=i+strlen(temp)+strlen(get_a(buff+len+1,'\n'));
        }
    }

    close(fd);

    return 0;
}
