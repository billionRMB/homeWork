/*************************************************************************
	> File Name: client.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月05日 星期二 19时26分31秒
 ************************************************************************/

#include"common/common.h"

char sIp[] = "192.168.2.86";
int port = 8731;
void initConfig(){
    port = 8731;
}

int main(){
    initConfig;
    write_Pi_log("test.log","port:%d,sIp %s\n",port,sIp);
    int cfd = connect_to_ip(sIp,port);

    return 0;
}


