#!/bin/bash -

if [ "$#" -ne 3 ];
then
	echo "[usage] $0 [elf_path] [module_name] [section]"
	echo "[section] can be .text \"\.data\" .bss .rodata .no_cache"
	exit
fi

i="0"
BIN_PATH="$1"
MODULE_NAME="$2"
SECTION_NAME="$3"
CMD=$(arm-eabi-nm -Srl -t d --format=sysv $BIN_PATH | grep $MODULE_NAME | grep $SECTION_NAME | cut -d '|' -f5 | sed 's/0*\([1-9]*\)/\1/' | less)

for j in $CMD
do
   i=$((i + $j))
done

echo [$SECTION_NAME] " " $MODULE_NAME " size = " $i " bytes"
