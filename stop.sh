#!/bin/sh

QServicePid=`ps -aef | grep QService | grep -v grep | awk '{print $2}'`
echo "QService pid:"$QServicePid
if [[ -n $QServicePid ]]
then
    kill -44 $QServicePid
fi

while true 
do
    QServicePid=`ps -aef | grep QService | grep -v grep | awk '{print $2}'`
    if [ -z $QServicePid ]
    then
        echo "QService stopped."
        break
    fi
    
    echo "wait QService stop..."
    sleep 1
done

