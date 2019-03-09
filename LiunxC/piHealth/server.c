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
    int sd;
}argment;

int port;
int cPort;
char*from;
char*to;
int INS;

char*configFile="common/pihealthd.conf";

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
    int sd = ((argment*)arg)->sd;
    int index = judgeINS(s); 
    DBG("插入用户%s到队列%d\n",ip,index);
    addCinfo(s->heads+index,cInfo);
    close(sd);
    return NULL;
}

void* heartBeat(void* arg){
    ServerControl*s = (ServerControl*)arg;
    clientInfo*p;
    qheads*tempHead;
    int n = 0;
    while(1){
        sleep(3);
        DBG("----------------------------------------\n");
        for(int i = 0;i < s->INS;i++){
            DBG("NO.%d心跳-队列[%d] 线上用户[%d]人\n", n,i,s->heads[i]. len);
            tempHead = s -> heads+i;
            p = tempHead->head;
            pthread_rwlock_rdlock(&tempHead->rwlock);
            // 注意这个坑
            while(p&&p->next){
                char*ip = strdup(inet_ntoa(p->next->saddr_client.sin_addr));
                if(connect_to_ip(ip,cPort)==-1){
                    pthread_rwlock_unlock(&tempHead->rwlock);
                    pthread_rwlock_wrlock(&tempHead->rwlock);
                    clientInfo*q = p -> next;
                    p->next = p -> next -> next;
                    tempHead -> len --;
                    free(q);
                    DBG("删除节点%s\n",ip);
                    pthread_rwlock_unlock(&tempHead->rwlock);
                    print(s);
                    pthread_rwlock_rdlock(&tempHead->rwlock);
                }else{
                    DBG("节点%s 还存在\n",ip);
                }
                p = p -> next;
            }
            pthread_rwlock_unlock(&tempHead->rwlock);
            }
        n++;
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
    }
}

void initConfig(ServerControl*s){
    //TODO 从配置文件中读取
    char*value;
    get_conf_value(configFile,"port",&value);
    port = atol(value);
    get_conf_value(configFile,"cPort",&value);
    cPort = atol(value);
    get_conf_value(configFile,"INS",&value);
    INS = atol(value);
    free(value);
    get_conf_value(configFile,"from",&from);
    get_conf_value(configFile,"to",&to);
}

int main(){
    ServerControl *s;

    initConfig(NULL);
    printf("port:%d\nclientPort:%d\nINS:%d\nfrom:%s\nto:%s\n",port,cPort,INS,from,to);

    initSCFL(&s,INS);
    initINSclient(s);

    print(s);

    int sd = make_server_socket(port,10);
    socklen_t len = sizeof(struct   sockaddr_in);

    pthread_t heartBeatP;
    // 心跳的线程
    pthread_create(&heartBeatP,NULL,heartBeat,(void*)s);
    while(1){
        int cfd;
        pthread_t pt;
        argment arg;
        clientInfo* cInfo = malloc(sizeof(clientInfo));
        if((cfd = accept(sd,(struct sockaddr*)&cInfo->saddr_client,&len) == -1)){
           perror("accept wrong ");
           continue;
        }
        char*ip = strdup(inet_ntoa(cInfo->saddr_client.sin_addr));

        DBG("IP:%s 用户登录\n",ip);

        if(findCinfo(s,ip,NULL)){
            DBG("IP:%s 用户已经登录\n",ip);
            continue;
        }
        arg.scf = s;
        arg.cInfo = cInfo;
        arg.sd = cfd;
        pthread_create(&pt,NULL,serverAccept,(void*)&arg);
    }
    
    return 0;
}
