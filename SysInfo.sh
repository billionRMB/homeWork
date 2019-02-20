#!/bin/bash
#处理小数运行的函数
function aExpb(){
    a=$1
    printf "%0.2f\n" `echo "scale=4 ; $1"|bc`
}
# 这个函数将秒数转换为时分秒
function sToHMS(){
    day=$(($1/86400))
    hour=$(($1%86400/3600 ))
    min=$(( ($1%3600)/60 ))
    echo up_${day}_days,_${hour}_hours,_${min}_minutes
}
# 用来比较浮点数大小
function floatBiger(){
    a=$1
    b=$2
    if [[ `echo "$a > $b" | bc -l` -eq 1 ]];then
        echo 1
    else
        echo 0
    fi
}
# 将数据转换为等级
function echoLevel(){
    #第一个参数数据大小
    temp=$1
    level1=$2
    level2=$3
    if [[ `floatBiger $level1 $temp` -eq 1 ]];then
        echo Normal
    elif [[ `floatBiger $level2 $temp` -eq 1 ]];then
        echo Note
    else
        echo Danger
    fi
}

curTime=`date +"%Y-%m-%d__%H:%M:%SS"`
hostName=`hostname`
osVersion=`cat /etc/issue`
kernelVersion=`uname -a | cut -f 3 -d" "`
runTimeS=`cat /proc/uptime | cut -f 1 -d "."`
avgLoad=`uptime|cut -f 3- -d "," |cut -f 2 -d ":"`
cpuTemp=$[`cat /sys/class/thermal/thermal_zone0/temp`]
cpuTemp=`aExpb $cpuTemp/1000`

#获得磁盘总量和用量
eval $(df | awk 'NR!=1{sum+=$2;used+=$3}END{printf("sum=%d Aused=%d",sum,used)}')
#获得内存总量和使用量
eval $(free | awk 'NR==2{printf("memTotal=%s memUsed=%.2f",$2,$3/$2*100)}')

Aused=`aExpb $Aused/$sum*100`

echo $curTime $hostName $osVersion $kernelVersion `sToHMS $runTimeS` $avgLoad $sum $Aused% $memTotal $memUsed% $cpuTemp `echoLevel $cpuTemp 50 70` `echoLevel $used 80 90` `echoLevel $memUsed 70 80`
