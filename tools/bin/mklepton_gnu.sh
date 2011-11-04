#!/bin/bash -

TMP_FILE=/tmp/mkconf.xml.tmp
SED=/bin/sed

#replace $HOME by its real value
$SED "s:\$(HOME):$HOME:g" $3 > $TMP_FILE

./mklepton_gnu $1 $2 $TMP_FILE

#delete tmp file
#rm -rf $TMP_FILE

