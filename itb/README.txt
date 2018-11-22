准备工作参考openwrt的README

两个config文件，openwrt和内核的配置文件，linux.config和openwrt.config，分别在make menuconfig 和make kernel_menuconfig时使用。

编译完生成的Image和dtb文件复制到目录，然后执行./mkitb.sh，即生成内核用的itb文件。