# !lsh
echo 'fat format use a 2Go partition'
mkfs -t vfat -n 4000 -b 65520 -s 32768 -d /dev/hd/sd0
mkdir /fattst
echo 'Before mount'
mount -t vfat /dev/hd/sd0 /fattst
tstfat
exit
