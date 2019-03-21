/*************************************************************************
	> File Name: server.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月05日 星期二 21时04分35秒
 ************************************************************************/
 
#include"common/common.h"

typedef struct argment{
    ServerControl* scf;
    clientInfo* cInfo;
    int index;
}argment;

int getFilePort;
int heartPort;
int chatPort;
char*from;
char*to;
int INS;

char*configFile="common/pihealthd.conf";
char*logFile="log/Master.log";
char**map;

void initMapConfig(){
    map = malloc(sizeof(char*)*10);
    for(int i = 0;i < 6;i ++){
        map[i] = malloc(sizeof(char)*20);
    }
    strcpy(map[0],"CpuLog.sh");
    strcpy(map[1],"MemLog.sh");
    strcpy(map[2],"DiskLog.sh");
    strcpy(map[3],"Users.sh");
    strcpy(map[4],"ProcLog.sh");
    strcpy(map[5],"SysInfo.sh");
    for(int i = 0;i < 6;i ++){
        printf("Map %d : %s\n",i,map[i]);
    }
}

void print(ServerControl* scf){
    printf("ServerControl->INS:%d\n",scf->INS);

    for(int i = 0;i < scf->INS;i++){
        clientInfo*p = scf -> heads[i].head;
        int no = 0;
        while(p -> next){
            printf("%d-%d IP : [%s]\n",i,no++,inet_ntoa(p -> next ->saddr_client.sin_addr));
            p = p -> next;
        }
    }
}

int judgeINS(ServerControl*s){
    int temp = 0;
    for(int i = 0;i < s->INS;i ++){
        temp = (s->heads[i].len < s->heads[temp].len ? i:temp);
    }
    return temp;
}

void* serverAccept(void * arg){
    ServerControl*s = ((argment*)arg)->scf;
    clientInfo*cInfo = ((argment*)arg)->cInfo;
    char*ip = strdup(inet_ntoa(cInfo->saddr_client.sin_addr));

    int index = judgeINS(s); 
    DBG("插入用户%s到队列%d\n",ip,index);
    addCinfo(s->heads+index,cInfo);
    return NULL;
}

void* heartBeatNoBlock(void*arg){
    int len = sizeof(errno);
    ServerControl* s = (ServerControl*) arg;
    clientInfo* p; qheads* tempHead;
    // 文件描述符集合
    fd_set fds;
    // 超时时间设置
    // 0.5 秒
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 1000000 * 0.5;
    int n = 0;
    while(1){
        //DBG("----------------------------\n");
        for(int i = 0;i < s->INS;i ++){
            DBG("NO.%d心跳-队列[%d] 线上用户[%d]人\n", n++,i,s->heads[i]. len);
            tempHead = s -> heads+i;
            p = tempHead->head;
            pthread_rwlock_rdlock(&tempHead->rwlock);
            // 注意这个坑
            while(p&&p->next){
                char*ip = strdup(inet_ntoa(p->next->saddr_client.sin_addr));
                int sfd = connect_to_ip_no_block(ip,heartPort);
                if(sfd == -1){
                    perror("ssssss - connect to ip no block:");
                    p = p -> next;
                    continue;
                }
                FD_ZERO(&fds);
                FD_SET(sfd,&fds);
                int retval = select(sfd+1,&fds,&fds,NULL,&tv);
                if(retval == -1){
                    perror("select(): ");
                }else if(retval == 0){
                    DBG("%s:连接超时\n",ip);
                    p = p -> next;
                }else {
                    getsockopt(sfd,SOL_SOCKET,SO_ERROR,&errno,(socklen_t*)&len);
                    if(errno != 0){
                        pthread_rwlock_unlock(&tempHead->rwlock);
                        pthread_rwlock_wrlock(&tempHead->rwlock);
                        clientInfo*q = p -> next;
                        p->next = p -> next -> next;
                        tempHead -> len --;
                        free(q);
             //           DBG("删除节点%s\n",ip);
                        pthread_rwlock_unlock(&tempHead->rwlock);
                        pthread_rwlock_rdlock(&tempHead->rwlock);
                    }else{
              //          DBG("节点%s 还存在\n",ip);
                        p = p -> next;
                    }
                }
                close(sfd);
            } 
            pthread_rwlock_unlock(&tempHead->rwlock);
        }
        sleep(1);
    }
}

// 分配所有的IP
void initINSclient(ServerControl*s){
    struct in_addr addrFrom;
    struct in_addr addrTo;
    inet_aton(from,&addrFrom);
    inet_aton(to,&addrTo);
    unsigned int nfrom = ntohl(addrTo.s_addr);
    unsigned int nto =  ntohl(addrFrom.s_addr);
    for(unsigned i = nto;i <= nfrom;i ++){
        clientInfo* cInfo = malloc(sizeof(clientInfo));
        cInfo->saddr_client.sin_addr.s_addr = htonl(i);
        DBG("生成配置IP:%s\n",inet_ntoa(cInfo->saddr_client.sin_addr));
        cInfo->next = NULL;
        addCinfo(s->heads+i%INS,cInfo);
        char filePath[20] = {0};
        sprintf(filePath,"file/%s",inet_ntoa(cInfo->saddr_client.sin_addr));
        mkdir(filePath,S_IRWXU);
    }
}

void initConfig(ServerControl*s){
    char*value;
    get_conf_value(configFile,"heartPort",&value);
    heartPort = atol(value);
    get_conf_value(configFile,"getFilePort",&value);
    getFilePort = atol(value);
    get_conf_value(configFile,"chatPort",&value);
    chatPort = atol(value);
    get_conf_value(configFile,"INS",&value);
    INS = atol(value);
    free(value);
    get_conf_value(configFile,"from",&from);
    get_conf_value(configFile,"to",&to);
}

// 从socket里接受数据存到FILE* 里
void saveToFile(int sd,FILE*f){
    char c;
    int n = 0;
    while((n = recv(sd,&c,sizeof(c),0))>0){
        if(c!=EOF){
            fputc(c,f);
        }else{
            break;
        }
    }
}

void chatToclient(char* ip){
    int fd;
    if((fd = connect_to_ip(ip,chatPort))<0){
        printf("Wrong connect %s:%d\n",ip,chatPort);
        return ;
    }
    for(int i = 0;i < 6;i ++){
        // 发送信息
        fileMessage message; 
        message.size = random()%1000;
        sprintf(message.name,"Master");
        sprintf(message.content,"%s",map[i]);
        sendMessage(fd,&message);
        printf("发送信息ok\n");
        // 接收信息
        recvMessage(fd,&message);
        // printf("recv:\nname:%s\ncontent[%lu]:%s\n",message.name,strlen(message.content),message.content);
        // 如果收到了OK就回一个ok然后等待接受文件
        if(strcmp(message.content,"OK") == 0){
            char filePath[100] = {0};
            sprintf(filePath,"file/%s/%s.log",ip,map[i]);
            printf("Start write to file %s\n",filePath);

            FILE*f =  fopen(filePath,"a+");
            sprintf(message.name,"Master");
            sprintf(message.content,"give me");
            sendMessage(fd,&message);
            saveToFile(fd,f);
            fclose(f);

            DBG("ok saveToFile %s\n",filePath);
        }else{
            // 记录一下
            write_Pi_log(logFile,"%s say:%s\n",message.name,message.content);
            DBG("somethings Wrong\n");
        }
    }
    close(fd);
    return ;
}
// 工作函数 
void* work(void* arg_){
    argment* arg = (argment*) arg_;
    while(1){
        sleep(30);
        qheads*head = &arg -> scf -> heads[arg->index]; 
        clientInfo* p = arg ->scf -> heads[arg->index].head;
        pthread_rwlock_rdlock(&head->rwlock);
        while(p -> next){
            char* ip = strdup(inet_ntoa(p -> next -> saddr_client.sin_addr));
            DBG("----------------------------------------------------\n");
            printf("** start to chat to %s ** \n",ip);
            chatToclient(ip);
            DBG("** end chat to %s ** \n",ip);
            p = p -> next;
        }
        pthread_rwlock_unlock(&head->rwlock);
    }
}

void* listenWarnigInfo(void*arg){

    int sid = socket(AF_INET,SOCK_DGRAM,0);
    if(sid < 0){
        perror("wrong create socket");
        return NULL;
    }

    struct sockaddr_in addr;
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(*(int *)arg);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int yes = 1;
    if(setsockopt(sid,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) < 0){
        close(sid);
        perror("setsockopt ");
        return NULL;
    }

    if(bind(sid,(struct sockaddr*)&addr,sizeof(addr)) < 0){
        close(sid);
        perror("bind");
        return NULL;
    }

    while(1){
        printf("start listenWarnigInfo_t \n");
        char recv[10] = {0};
        socklen_t len = sizeof(addr);
        recvfrom(sid,recv,10,0,(struct sockaddr*)&addr,&len);
        printf("\033[47;31mWaring\033[0m:%s\n",recv);
    }

    close(sid);

    return NULL;
}

int main(){
    initMapConfig();

    ServerControl *s;

    initConfig(NULL);
    printf("getFilePort:%d\nchatPort:%d\nheartPort:%d\nINS:%d\nfrom:%s\nto:%s\n",getFilePort,chatPort,heartPort,INS,from,to);

    initSCFL(&s,INS);
    initINSclient(s);

    print(s);

    int sd = make_server_socket(chatPort,10);
    socklen_t len = sizeof(struct sockaddr_in);

    pthread_t heartBeatP;
    // 心跳的线程
    pthread_create(&heartBeatP,NULL,heartBeatNoBlock,(void*)s);
    
    // 开始工作
    pthread_t* workIns = calloc(INS,sizeof(pthread_t));

    for(int i = 0;i < INS;i ++){
        argment*arg = malloc(sizeof(argment));
        arg->scf = s;
        arg->index = i; 
        pthread_create(&workIns[i],NULL,work,(void*)arg);
    }

    int lsWport = 8686;
    // 监听UDP
    pthread_t listenWarnigInfo_t;
    pthread_create(&listenWarnigInfo_t,NULL,listenWarnigInfo,(void*)&lsWport);

    while(1){
        int cfd;
        pthread_t pt;
        argment*arg = malloc(sizeof(argment));
        clientInfo* cInfo = malloc(sizeof(clientInfo));
        if((cfd = accept(sd,(struct sockaddr*)&cInfo->saddr_client,&len) == -1)){
            perror("accept wrong ");
            free(cInfo);
            close(cfd);
            continue;
        }
        char*ip = strdup(inet_ntoa(cInfo->saddr_client.sin_addr));
        
        DBG("IP:%s 用户登录\n",ip);
        
        if(findCinfo(s,ip,NULL)){
            DBG("IP:%s 用户已经登录\n",ip);
            continue;
        }
        arg->scf = s;
        arg->cInfo = cInfo;
        // 加入节点
        pthread_create(&pt,NULL,serverAccept,(void*)arg);
        close(cfd);
    }
    
    return 0;
}
