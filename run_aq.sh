#!/bin/bash

./exhaust_memory &
pid=`ps aux | grep exhaust_memory | grep -v grep | awk '{print $2}'`
echo $pid $1
cgroup=`cat /proc/$pid/cgroup`
echo $cgroup
sleep 3
./aqclient $pid $1

sudo kill -9 $pid
