#!/bin/bash
Time=`date +"%Y-%m-%d__%H:%M:%S"`
df|grep /dev/sd | awk -v time=$Time '{sumAll+=$2;sumUsed+=$3;printf("%s 1 %s %s %s %f%%\n",time,$6,$2,$3,$3/$2*100)}END{printf("%s 0 disk %s %s %s%%\n",time,sumAll,sumUsed,sumUsed/sumAll*100)}'
