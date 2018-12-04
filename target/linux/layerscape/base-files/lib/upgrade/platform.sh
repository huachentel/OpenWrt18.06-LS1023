#!/bin/sh

PART_NAME=firmware

platform_check_image() {
	return 0;
}

platform_do_upgrade() {
	local board=$(board_name)

	case "$board" in
	fsl,ls1043a-rdb)
		default_do_upgrade "$ARGV"
		;;
	*)
		echo "Sysupgrade is not yet supported on ${hardware}."
		return 1
		;;
	esac
}