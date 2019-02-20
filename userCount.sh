#!/bin/bash
curTime=`date +"%Y-%m-%d__%H:%M:%SS"`
userTree='['`last| grep -v 'wtmp begins'| grep -v reboot | cut -f 1 -s -d " " |uniq|head -n 3 |tr '\n' ' '`']'
rootUser='['`cat /etc/passwd | grep 1000 |cut -f 1 -d ":"`']'
users=`w -h|awk 'BEGIN{printf("[")}{printf("%s_%s_%s,",$1,$3,$2)}END{printf("]")}'`
userNum=`w -h|wc -l`
#去除空格。。神奇
#userNum=`${userNum// /}
echo $curTime $userNum $userTree $rootUser $users 
