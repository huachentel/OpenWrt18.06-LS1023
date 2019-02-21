#!/bin/sh
ethn=wlan0
wlan_led_flag=0

echo "timer" > /sys/class/leds/wifi_5g/trigger && sleep 5 
echo "0" > /sys/class/leds/wifi_5g/delay_on && sleep 1
echo "1000" > /sys/class/leds/wifi_5g/delay_off && sleep 1

while true
do
 wlan_flag=$(ifconfig | grep $ethn)
 if [ "$wlan_flag" ]; then
	if [ "$wlan_led_flag" -eq 0 ]; then
	echo "1000" > /sys/class/leds/wifi_5g/delay_on
	echo "0" > /sys/class/leds/wifi_5g/delay_off    
	wlan_led_flag=1
	fi
	 RX_pre=$(cat /proc/net/dev | grep $ethn | sed 's/:/ /g' | awk '{print $2}')
	 TX_pre=$(cat /proc/net/dev | grep $ethn | sed 's/:/ /g' | awk '{print $10}')
	 sleep 1
	 RX_next=$(cat /proc/net/dev | grep $ethn | sed 's/:/ /g' | awk '{print $2}')
	 TX_next=$(cat /proc/net/dev | grep $ethn | sed 's/:/ /g' | awk '{print $10}')

	 RX=$((${RX_next}-${RX_pre}))
	 TX=$((${TX_next}-${TX_pre}))
	sleep 1 
	if [ "$TX" -eq 0 ]; then
	  if [ "$RX" -eq 0 ]; then
		echo "1000" > /sys/class/leds/wifi_5g/delay_on
		echo "0" > /sys/class/leds/wifi_5g/delay_off
	  else
		echo "500" > /sys/class/leds/wifi_5g/delay_on
		echo "500" > /sys/class/leds/wifi_5g/delay_off
	  fi
	else
		echo "500" > /sys/class/leds/wifi_5g/delay_on
		echo "500" > /sys/class/leds/wifi_5g/delay_off
	fi
    else
	echo "0" > /sys/class/leds/wifi_5g/delay_on
	echo "1000" > /sys/class/leds/wifi_5g/delay_off
fi
done