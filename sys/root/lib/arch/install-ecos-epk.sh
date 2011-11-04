#!/bin/bash

TAUON_HAL_PATH=$1"/hal"
TAUON_HAL_ECC=$1"/ecos.cc"

TAUON_HAL_TAR_NAME="tauon-ecos-hal.tar"
TAUON_HAL_TARGZ_NAME=$TAUON_HAL_TAR_NAME".gz"
TAUON_HAL_EPK_NAME="tauon-ecos-hal.epk"

TAUON_CURRENT_DIRECTORY=$PWD


if [ -z "$1" ] ; then
   printf "arguments needed: hal path\n";
   exit 1
fi

if [ ! -e $TAUON_HAL_PATH ]; then
   printf "path %s not exist\n" $TAUON_HAL_PATH
   exit 2
fi 

cd $1 

#clean
rm $TAUON_HAL_EPK_NAME
rm $TAUON_HAL_TAR_NAME
rm $TAUON_HAL_TARGZ_NAME

#create epk
tar vcf $TAUON_HAL_TAR_NAME --exclude .svn hal *.db
gzip -v $TAUON_HAL_TAR_NAME
mv $TAUON_HAL_TARGZ_NAME $TAUON_HAL_EPK_NAME

printf "epk %s is generated\n" $TAUON_HAL_EPK_NAME 

if [ -z "$2" ] ; then
   exit 0;
fi

printf "add epk %s from %s to eCos configtool\n" $TAUON_HAL_EPK_NAME $TAUON_HAL_PATH

#remove previous package
$ECOS_REPOSITORY/ecosadmin.tcl remove $2

#remove new package
$ECOS_REPOSITORY/ecosadmin.tcl add $TAUON_HAL_EPK_NAME

