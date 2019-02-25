/*************************************************************************
	> File Name: socketCS.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年02月23日 星期六 18时07分29秒
 ************************************************************************/
#include "common/socklib.h"
int main(){
    int pid = fork();
    if(pid == 0){
        execl("./server","./server","8732",NULL);
    }else{
        execl("./client","./client","192.168.1.40","8731","银.TAMA",NULL);
    }
    return 0;
}
