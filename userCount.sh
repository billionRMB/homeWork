#!/bin/bash
curTime=`date +"%Y-%m-%d__%H:%M:%SS"`
userTree='['`last |grep -v root |grep -v reboot|grep -v wtmp|cut -f 1 -d " "|sort|uniq -c|sort -r -t " " -k 1|head -3|awk '{printf("%s ",$2)}'`']'
rootUser='['`cat /etc/group | grep sudo|cut -f 4 -d ":"`']'
users=`w -h|awk 'BEGIN{printf("[")}{printf("%s_%s_%s,",$1,$3,$2)}END{printf("]")}'`
userNum=`w -h|wc -l`
#去除空格。。神奇
#userNum=`${userNum// /}
echo $curTime $userNum $userTree $rootUser $users 
