#!/bin/bash -

if [ "$#" != "1" ];
then
	echo "$0 [/absolute/bin_path]"
	exit 0
fi

if [ -h "bin" ];
then
	unlink bin
	echo "Remove symlink bin"
fi

ln -s "$1" `basename "$1"`
