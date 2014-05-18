#!/bin/bash -

HOST_ADDR=$(/sbin/ifconfig | grep inet | head -1| cut -d ':' -f 2 | cut -d ' ' -f 1)
ROUTE_ADDR=$(/sbin/route -n | tail -1 | tr -s ' ' ':' | cut -d ':' -f 2)

#stop virtual network interface
/sbin/ifconfig tap0 down
/sbin/ifconfig br0 down

tunctl -d tap0

#delete bridge
/usr/sbin/brctl delbr br0

#reconfigure eth0
/sbin/ifconfig eth0 $HOST_ADDR -promisc up

route add default gw $ROUTE_ADDR

#restart network
#/etc/init.d/networking restart

#
/sbin/ifconfig -a
