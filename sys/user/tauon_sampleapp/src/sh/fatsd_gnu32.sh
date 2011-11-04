# !lsh
echo 'fat format use a 2Go partition'
mkfs -t vfat -n 4000 -b 65520 -s 32768 -d /dev/sdcard0
mkdir /fattst
echo 'Before mount'
mount -t vfat /dev/sdcard0 /fattst
exit
