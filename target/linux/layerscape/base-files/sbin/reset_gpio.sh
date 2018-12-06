#!/bin/sh

if [ ! -d "/sys/class/gpio/gpio475" ];then
	echo 475 > /sys/class/gpio/export
	echo in > /sys/class/gpio/gpio475/direction
fi

while [ "1" = "1" ]
do
	num=$(cat /sys/class/gpio/gpio475/value)
	if [ "$num" = "0" ];
		then
			sleep 3
			num=$(cat /sys/class/gpio/gpio475/value)
			if [ "$num" = "0" ];then
				echo "reboot..........."
				mkdir /recover && mount /dev/mmcblk0p3 /recover && touch /recover/recover && umount /recover && rm -rf /recover && sync
				reboot
			fi
	fi
	sleep 1;
done
