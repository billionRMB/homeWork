/*************************************************************************
	> File Name: 2.socketServer.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年02月23日 星期六 18时46分52秒
 ************************************************************************/

#include "common/socklib.h"

void send_to_fd(int fd,message*msg){
    sendMessage(fd,msg);
}
void send_to_name(SCFL*s,char*name,message*msg){
    int sendfd = findCinfo(s,name)->next->socketFd;
    send_to_fd(sendfd,msg);
}

void send_all(SCFL*s,message*msg){
    clientInfo*p = s -> cInfo;
    while(p -> next){
        send_to_fd(p ->next->socketFd,msg);
        p = p -> next;
    }
}
int doClient(SCFL*sCF,clientInfo*cInfo){
    // 取到姓名
    recv(cInfo->socketFd, cInfo->name, sizeof(cInfo->name), 0);
    if(findCinfo(sCF,cInfo->name)->next)return 1;
    cInfo -> next = NULL;
    addCinfo(sCF,cInfo);
    print(sCF);
}

char* getAll(clientInfo*c,int num){
    char* result = calloc(22*num,sizeof(char));
    clientInfo*p = c;
    int end = 0;
    while(p->next){
        printf("p:%s\n",p->next->name);
        int i = strlen(p -> next -> name);
        strncpy(result+end,p -> next -> name,i);
        result[end+i++] = ' ';
        end += i;
        p = p -> next;
    }
    return result;
}

void* process_request(void* argc){
    argment* arg = (argment*) argc;
    clientInfo*p = arg->cInfo;
    char welcome[1024];
    message msg;
    strcpy(msg.from, p->name);
    sprintf(msg.message, "%s 您已登录服务器!", p->name);
    msg.flag = 2;
    send_to_fd(p->socketFd, &msg);
    send_all(arg->scf, &msg);
    char buffer[1024] = {0};
    while (recv(p->socketFd, buffer, sizeof(buffer), 0) > 0) {
        message msg;
        printf("%s: %s\n", p->name, buffer);
        msg.flag = 0;
        strcpy(msg.message, buffer);
        strcpy(msg.from, p->name);
        if (buffer[0] == '#') {
            sprintf(msg.message,"%d个人在线:%s",arg->scf->num,getAll(arg->scf->cInfo,arg->scf->num));
            send_to_fd(p->socketFd,&msg);
            printf("message:%s\n",msg.message);
        } else if (buffer[0] == '@') {
            char to[20] = {0};
            int i = 0;
            for (; i < 20; i++) {
                if (buffer[i] == ' ') break;
            }
            strncpy(to, buffer + 1, i - 1);
            if (findCinfo(arg->scf,to)->next){
                msg.flag = 1;
                send_to_name(arg->scf,to, &msg);
            } else {
                sprintf(msg.message,"非法用户名");
                msg.flag = 1;
                send_to_fd(p->socketFd,&msg);
            }
        } else {
            printf("messag:%s\n",msg.message);
            send_all(arg->scf, &msg);
        }
        memset(buffer, 0, sizeof(buffer));
    }
    printf("用户 %s 已下线！\n", p->name);
    deleteCinfo(arg->scf, p->name);
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
    serverCF->doClient = doClient;
    serverCF->process_request = process_request;
    // 运行server
    runServer(serverCF,port);
    return 0;
}
