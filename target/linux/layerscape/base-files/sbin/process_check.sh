#!/bin/sh

while [ "1" = "1" ]
do
	count=`ps |grep reset_gpio.sh |grep -v "grep" |wc -l`
    if [ 0 == $count ];then
		echo "start reset_gpio.sh"
		/sbin/reset_gpio.sh &
    fi
	sleep 60
done