cd $1/itb

cp linux-4.9.111/arch/arm64/boot/Image ./
gzip Image
mkimage -f linux_arm64.its lsdk_linux_arm64_tiny.itb
sudo cp lsdk_linux_arm64_tiny.itb $1/target/linux/layerscape/base-files/
