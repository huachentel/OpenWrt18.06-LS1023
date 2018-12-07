#! /bin/sh

echo "waiting for 10 second rebooting system and do update"
cd /
# 在分区p3中设置升级标识recover文件
mkdir /recover && mount /dev/mmcblk0p3 /recover && touch /recover/recover && sync 
# 保存用户当前配置,并置配置恢复标志
sysupgrade -b /recover/backup.tar.gz && touch /recover/sys_etc_recover
umount /recover && rm -rf /recover

reboot