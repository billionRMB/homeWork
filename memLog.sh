#!/bin/bash
last=$1
curTime=`date +"%Y-%m-%d__%H:%M:%SS"`
memTotal=`cat /proc/meminfo | grep MemTotal|awk '{printf("%s",$2)}'`
memFree=`cat /proc/meminfo | grep MemFree|awk '{printf("%s",$2)}'`

#处理小数运行的函数
function aExpb(){
    a=$1
    printf "%0.2f\n" `echo "scale=2 ; $1"|bc` 
}

memTotal=`aExpb $memTotal/1024`
memFree=`aExpb $memFree/1024`
curP=`aExpb $memFree/$memTotal`
curP=`aExpb $curP*100`

echo $curTime ${memTotal}M ${memFree}M ${curP}% `aExpb $last*0.3+0.7*$curP`%
