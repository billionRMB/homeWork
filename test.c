/*************************************************************************
	> File Name: test.c
	> Author:Gin.TaMa 
	> Mail:1137554811@qq.com 
	> Created Time: 2019年02月20日 星期三 19时49分31秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
int main(){
    for(int x = -3;x <= 3;x ++){
        for(int y = 3;y >= -3;y--){
            putchar(abs(x)+abs(y)<=3?'*':' ');
        }
        putchar('\n');
    }
    return 0;
}
