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
        return -1;
    }

    struct sockaddr_in their_add;
    their_add.sin_family = AF_INET;
    their_add.sin_port = htons(port);
    their_add.sin_addr.s_addr=inet_addr(IP);
    bzero(&(their_add.sin_zero),8);

    if(connect(sockfd,(struct sockaddr*)&their_add,sizeof(struct sockaddr)) == -1){
       perror("connect server failed:");  
       close(sockfd);
       return -1;
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

int get_conf_value(char *pathname, char *key_name,char** value){
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
        int tempLen = strlen(temp);
        if(strncmp(key_name,temp,len) == 0){
            *value=get_a(buff+i+len+1,'\n');
            break;
        }else{
            i=i+tempLen+2+strlen(get_a(buff+i+tempLen+1,'\n'));
        }
    }

    close(fd);

    if(i>=length){
        printf("%s is not right key\n",key_name);
        exit(-1);
    }

    return 0;
}

void initSCFL(SCFL**sl){
    *sl = calloc(1,sizeof(SCFL));
    (*sl) -> num = 0;
    (*sl) -> cInfo = calloc(1,sizeof(clientInfo));
    (*sl) -> cInfo -> next = NULL;
    (*sl) -> tail = (*sl) -> cInfo;
    pthread_rwlock_init(&((*sl)->rwlock),NULL);
}

void addCinfo(SCFL*s,clientInfo*node){
    pthread_rwlock_wrlock(&s->rwlock);
    s->num++;
    s->tail->next = node;
    s->tail = node;
    node->next = NULL;
    pthread_rwlock_unlock(&s->rwlock);
}

clientInfo* findCinfo(SCFL*s,char* name){
    pthread_rwlock_rdlock(&s->rwlock);
    clientInfo* p = s -> cInfo;
    while(p -> next!= NULL && strcmp(p->next-> name,name)){
        printf("find:%s:%s\n",p -> next -> name,name);
        p = p -> next;
    }
    pthread_rwlock_unlock(&s->rwlock);
    return p;
}

void deleteCinfo(SCFL*s,char* name){
    clientInfo*p = findCinfo(s,name);
    pthread_rwlock_wrlock(&s->rwlock);
    if(p->next == NULL)return;
    clientInfo*q = p -> next;
    p -> next = p -> next -> next;
    free(q);
    s -> num --;
    pthread_rwlock_unlock(&s->rwlock);
    print(s);
}

void setTips(SCFL*sl,char*tips){
    sl->tips = calloc(strlen(tips),sizeof(char));
    strcpy(sl->tips,tips);
}

int runServer(SCFL* serverCF,int mport){
    char filePath[]="./common/server.conf";
    // 从文件中读取配置
    char* tempport = NULL;
    get_conf_value(filePath,"server_port",&tempport);
    // 获得连接
    if(mport==-1){
        mport = strToInt(tempport);
    }
    printf("Server run on ");
    DBG(mport);
    int socket_fd = make_server_socket(mport,10);
    int connect_fd;
    // 输出server 的提示
    printf("%s",serverCF->tips);
    // 客户端的一部分信息
    while(1){
        pthread_t pt;
        argment arg;
        clientInfo* cInfo = malloc(sizeof(clientInfo));
        if( (connect_fd = accept(socket_fd, (struct sockaddr*)&cInfo->saddr_client,(socklen_t*)&cInfo->clength)) == -1){
        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
        exit(0);
        }
        cInfo -> socketFd = connect_fd;
        if(serverCF->doClient(serverCF,cInfo)==1)continue;
        arg.scf = serverCF;
        arg.cInfo = cInfo;
        pthread_create(&pt,NULL,serverCF->process_request,(void*)&arg); 
    }
    close(socket_fd);
}

#define BUFFER_SIZE 1024

int sendMessage(int sockfd,message* msg){
    // 将msg拷贝到char数组中然后发送
    int needSend=sizeof(message);
    char *buffer=(char*)malloc(needSend);
    memcpy(buffer,msg,needSend);
    int pos=0;
    int len=0;
    while(pos < needSend)
    {
        len=send(sockfd, buffer+pos, BUFFER_SIZE,0);
        if(len <= 0)
        {
            perror("ERRPR");
            break;
        }
        pos+=len;
    }
    free(buffer);
    return needSend;
}
int recvMessage(int connetfd,message* msg){
    int needRecv = sizeof(message);
    char *buffer=(char*)malloc(needRecv);
    int pos = 0,len = 0;
    while(pos < needRecv){
        len = recv(connetfd,buffer+pos,BUFFER_SIZE,0);
        if(len < 0){
            perror("server recv data wrong");
            return -1;
        }
        pos+=len;
    }
    memcpy(msg,buffer,needRecv);
    free(buffer);
    return needRecv;
}

void print(SCFL* scf){
    int i = 0;
    clientInfo*p = scf -> cInfo;
    while(p -> next){
        printf("%d:name : [%s]\n",i++,p -> next -> name);
        p = p -> next;
    }
}

void send_to_fd(int fd,message*msg){
    sendMessage(fd,msg);
}

void send_to_name(SCFL*s,char*name,message*msg){
    int sendfd = findCinfo(s,name)->next->socketFd;
    send_to_fd(sendfd,msg);
}

void send_all(SCFL*s,message*msg){
    clientInfo*p = s -> cInfo;
    pthread_rwlock_wrlock(&s->rwlock);
    while(p -> next){
        send_to_fd(p ->next->socketFd,msg);
        p = p -> next;
    }
    pthread_rwlock_unlock(&s->rwlock);
}
char* getAll(SCFL*s,int num,char endc){
    char* result = calloc(22*num,sizeof(char));
    clientInfo*p = s->cInfo;
    int end = 0;
    pthread_rwlock_rdlock(&s->rwlock);
    while(p->next){
        int i = strlen(p -> next -> name);
        strncpy(result+end,p -> next -> name,i);
        result[end+i++] = endc;
        end += i;
        p = p -> next;
    }
    pthread_rwlock_unlock(&s->rwlock);
    return result;
}

