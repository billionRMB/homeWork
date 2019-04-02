#!/bin/bash
for(( i=1;i<=10;i++ ));
do
    ssh gin.tama@pi$i
    scp ~/.ssh/id_rsa.pub gin.tama@pi$i:~/.ssh/a.out
    #cat ~/.ssh/a.out >> ~/.ssh/authorized_keys
    #echo "ok"
done
