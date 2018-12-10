准备工作参考openwrt的README

两个config文件，openwrt和内核的配置文件，linux.config和openwrt.config，分别在make menuconfig 和make kernel_menuconfig时使用。

20181207改动：
1. 将itb与openwrt文件系统打包的时候合二为一，方便进行烧写。

2. 新增加了sysupgrade命令，用于进行配置的备份和还原。

3. 新增加了reset按键功能和sys_update功能，用于恢复系统和升级系统功能。

4. ramdisk模式的系统改成了精简版的openwrt,支持ssh，用于紧急情况的系统恢复。ramdisk底下的恢复为：
（1）uboot下设置：
	setenv bootargs "console=ttyS0,115200 root=/dev/ram0 earlycon=uart8250,mmio,0x21c0500"
	setenv bootcmd "run distro_bootcmd; run sd_bootcmd; env exists secureboot && esbc_halt"
	boot
（2）在/dev/mmcblk0p3分区放入系统镜像，并创建空文件touch recover
（3）重启，系统会自动进行还原

5. 改进了firmware,位于itb/firmware目录，添加了对在线升级的底层支持

6. 重新对各网口命名使之对应面板编号，从iface0~iface4

一、openwrt根目录执行完make V=s -j1编译后会在bin/targets/layerscape/armv8_64b-glibc下面生成
openwrt-layerscape-armv8_64b-ls1043ardb-squashfs-sysupgrade.bin,也就是内核和文件系统的打包
二、升级
新改动的升级功能分两种，一种是还原分区（不保存当前配置），一种是直接升级（保存当前配置）

（1）将升级文件放到/dev/mmcblk0p3分区，长按reset键5s以上，系统会重启进行升级。将/dev/mmcblk0p3分区的镜像
恢复到sys分区也就是/dev/mmcblk0p1中。

（2）将升级文件openwrt-layerscape-armv8_64b-ls1043ardb-squashfs-sysupgrade.bin放到设备中，然后执行sysupgrade openwrt-layerscape-armv8_64b-ls1043ardb-squashfs-sysupgrade.bin,
系统会进行升级。

三、配置备份与还原(暂未实现)
（1）使用sysupgrade -b xxxx.tar.gz进行配置的备份
（2）使用sysupgrade -r xxxx.tar.gz进行配置的还原

四、说明
第一次更新完新的firmware之后最好执行第4节中的ramdisk紧急情况系统恢复，因为有可能系统分区此时没有系统。