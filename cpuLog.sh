#!/bin/bash

#处理小数运行的函数
function aExpb(){
    a=$1
    printf "%0.2f\n" `echo "scale=4 ; $1"|bc`
}
curTime=`date +"%Y-%m-%d__%H:%M:%SS"`
cpuNum=`cat /proc/cpuinfo| grep "physical id"|wc -l` 
loadavg=`cat /proc/loadavg | cut -f 1-3 -d " "`
cpuTemp=$[`cat /sys/class/thermal/thermal_zone0/temp`]
cpuTemp=`aExpb $cpuTemp/1000`

function floatBiger(){
    a=$1
    b=$2
    if [[ `echo "$a > $b" | bc -l` -eq 1 ]];then
        echo 1 
    else
        echo 0
    fi
}

function echoTemp(){
    #第一个参数温度
    temp=$1
    if [[ `floatBiger 50 $temp` -eq 1 ]];then
        echo Normal
    elif [[ `floatBiger 70 $temp` -eq 1 ]];then
        echo Note
    else
        echo Danger
    fi
}

eval $(head -n 1 /proc/stat | awk -v sum1=0 -v idel1=0 '{for(i=2;i<=8;i++){sum1=sum1+$i}}END{printf("sum1=%d idel1=%d",sum1,$5)}')
#　为啥呢？

sleep 0.5

eval $(head -n 1 /proc/stat | awk -v sum1=0 -v idel1=0 '{for(i=2;i<=8;i++){sum1=sum1+$i}}END{printf("sum2=%d idel2=%d",sum1,$5)}')

echo $curTime $loadavg ${cpuTemp}℃ " "$(aExpb "100*(1-($idel2-$idel1)/($sum2-$sum1))")%　$(echoTemp $cpuTemp)
