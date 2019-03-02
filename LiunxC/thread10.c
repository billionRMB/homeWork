/*************************************************************************
	> File Name: thread10.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年03月02日 星期六 09时05分55秒
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

struct arg_sum{
    int begin;
    int end;
};

int now = 0,sumall = 0,end;

pthread_mutex_t mutex;

void* sum2(){
    while(1){
        pthread_mutex_lock(&mutex);
        if(now > end){
            pthread_mutex_unlock(&mutex);
            break;
        }
        sumall+=now;
        now++;
        pthread_mutex_unlock(&mutex);
    }

}
/*
void* sum(void*arg_){
    struct arg_sum*arg = (struct arg_sum*)arg_;
    int ans =  0;
    printf("begin:%d:end:%d\n",arg->begin,arg->end);
    for(int i = arg->begin+1;i <= arg->end;i ++){
        ans += i;
    }
    printf("sum:%d\n",ans);
    return (void*)ans;
}
*/

int main(int arg,char*args[]){
    int allnum = atoi(args[1]);
    end = allnum;
    int nump = atoi(args[2]);
    int ans = 0;
    pthread_t*sum_t = malloc(nump*sizeof(pthread_t));
    printf("%d:%d\n",allnum,nump);
    for(int i = 0;i < nump;i ++){
        /*
        struct arg_sum*asum=malloc(sizeof(struct arg_sum));
        asum->begin = allnum/nump * i;
        asum->end = allnum/nump * (i+1);
        */
        pthread_create(sum_t+i,NULL,sum2,NULL);
    }
    for(int i = 0;i < nump;i ++){
        pthread_join(sum_t[i],NULL);
    }
    printf("ans:%d\n",end);
    return 0;
}
