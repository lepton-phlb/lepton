# !lsh
echo 'fat format use a 2Go partition'
mkfs -t vfat -n 4000 -b 65520 -s 32768 -d /dev/hd/sdhc0
mkdir /fattst
echo 'Before mount on /fattst'
mount -t vfat /dev/hd/sdhc0 /fattst
exit
