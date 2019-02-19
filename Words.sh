#!/bin/bash

# 初始化参数
path=$1
length=(0,0,0)
filename=(0,0,0)
strings=(0,0,0)
# 获得指定路径下所有的类型为ｆ的文件
files=`find $path -name "*" -type f`

function storeLong(){
    #$1 : 待判断的字符串
    #$2 : 该字符串所处的文件名
    str=$1
    tempFile=$2
    strN=${#str}
    for i in `seq 0 2`;do
        if [[ strN -ge length[$i] ]];then
            for j in `seq 2 -1 $i`;do
                length[$j]=${length[$j-1]}
                filename[$j]=${filename[$j-1]}
                strings[$j]=${filename[$j-1]}
            done
            length[$i]=$strN
            filename[$i]=$tempFile
            strings[$i]=$str
            break
        fi
    done    
}

for i in $files;do
    # 判断是否为text文件
    file $i | grep text > /dev/null
    if [[ $? -eq 0 ]];then
        # 获得文件中最长的字符串
        Lstr=`awk 'BEGIN{RS="[\n ]"}{if (length(max)<length()) max=$0}END{print max}' $i` 
        # 比较并保留最长的字符串
        storeLong ${Lstr} $i
    fi
done

# 输出结果
for i in `seq 0 2`;do
    echo ${length[$i]}----${filename[$i]}----${strings[$i]}
done
