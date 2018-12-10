#!/bin/sh

if [ ! -d "/sys/class/gpio/gpio475" ];then
	echo 475 > /sys/class/gpio/export
	echo in > /sys/class/gpio/gpio475/direction
fi


while [ "1" = "1" ]
do
	num=$(cat /sys/class/gpio/gpio475/value)
	if [ "$num" = "0" ]; then
		sleep 1
		num=$(cat /sys/class/gpio/gpio475/value)
		if [ "$num" = "0" ]; then
		/etc/rc.button/reset
		fi
	fi
	
	while [ "$num" = "0" ]
	do
		num=$(cat /sys/class/gpio/gpio475/value)
		sleep 1;
	done
	sleep 1;
done
