#!/bin/bash

if [ -z "$1" ] ; then
   printf "arguments needed: lib ecos ecc path\n";
   exit 1
fi

if [ -z "$2" ] ; then
   printf "arguments needed: lib ecos target path\n";
   exit 1
fi

ARG1_ABSOLUTE_PATH=$(readlink -f $1)
ARG2_ABSOLUTE_PATH=$(readlink -f $2)

TAUON_LIB_ECOS_PATH=$ARG1_ABSOLUTE_PATH"/lib"
TAUON_LIB_ECOS_INSTALL_PATH=$TAUON_LIB_ECOS_PATH"/install"
TAUON_LIB_ECOS_TARGET_PATH=$ARG2_ABSOLUTE_PATH
TAUON_LIB_ECOS_TARGET_INSTALL_PATH=$TAUON_LIB_ECOS_TARGET_PATH"/install"

TAUON_CURRENT_DIRECTORY=$PWD


if [ ! -e $TAUON_LIB_ECOS_PATH ]; then
   printf "path %s not exist\n" $TAUON_LIB_ECOS_PATH
   exit 2
fi 

if [ -L $TAUON_LIB_ECOS_TARGET_INSTALL_PATH ] ; then
     echo "remove previous target install link %s \n" TAUON_LIB_ECOS_TARGET_INSTALL_PATH
    unlink $TAUON_LIB_ECOS_TARGET_INSTALL_PATH
elif [ -d $TAUON_LIB_ECOS_TARGET_INSTALL_PATH ] ; then
    echo "remove previous target install directory  %s \n" TAUON_LIB_ECOS_TARGET_INSTALL_PATH
    rm -rf $TAUON_LIB_ECOS_TARGET_INSTALL_PATH
fi


#place into lib ecos directory
cd $TAUON_LIB_ECOS_PATH

#remove previous install directory
rm -rf install

ecosconfig check

echo "create build tree"
ecosconfig tree

echo "generate build tree"
make

echo "End ..."
cp  -v target.ld install/lib

echo "Delete tmp files .."
rm -rf hal kernel makefile services error infra isoinfra language ecos_build ecos_install ecos_mlt

printf "make symbolik link between\n %s\nand\n %s\n" $TAUON_LIB_ECOS_INSTALL_PATH $TAUON_LIB_ECOS_TARGET_INSTALL_PATH 
ln -s $TAUON_LIB_ECOS_INSTALL_PATH $TAUON_LIB_ECOS_TARGET_INSTALL_PATH

ls -alL $TAUON_LIB_ECOS_TARGET_INSTALL_PATH

