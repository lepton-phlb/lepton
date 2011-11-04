#!/bin/bash -

if [ "$#" -ne 1 ];
then
	echo "[usage] $0 [elf_path]"
	exit
fi

MODULES_NAME=" ecos-v3.0 \
kernel/core \
fs/vfs fs/rootfs fs/ufs fs/kofs fs/yaffs fs/fat \
dev/arch/all \
dev/arch/cortexm \
dev/arch/at91 dev/arch/arm9 \
dev/dev_cpufs dev/dev_fb dev/dev_head dev/dev_mem \
dev/dev_null dev/dev_part dev/dev_proc dev/dev_tty \
kernel/net/lwip \
lib/flnx-1.1.8 lib/libc lib/librt lib/nanox lib/pthread lib/TinyGL \
src/bin src/sbin"

SECTIONS_NAME=".text .rodata \"\.data\" .bss .no_cache"

BIN_PATH="$1"

COUNT_SCRIPT="$HOME/tauon/tools/host/debian/count_module_size.sh"

for module in $MODULES_NAME
do
	echo "==========="
	
	for section in $SECTIONS_NAME
	do
		eval $COUNT_SCRIPT $BIN_PATH $module $section
	done
	
	echo "***********"
done
