#!/bin/bash -

#get addr of eth0
HOST_ADDR=$(/sbin/ifconfig | grep inet | head -1| cut -d ':' -f 2 | cut -d ' ' -f 1)
ROUTE_ADDR=$(/sbin/route -n | tail -1 | tr -s ' ' ':' | cut -d ':' -f 2)
#create bridge
brctl addbr br0

#create tun/tap interface for root
tunctl -u root
ifconfig tap0 0.0.0.0 promisc up

#configure eth0 in promiscious mode
ifconfig eth0 0.0.0.0 promisc up

#add eth0 to bridge
brctl addif br0 eth0

#add tap0 to bridge
brctl addif br0 tap0

#give br0 IP address and give a default gw
ifconfig br0 $HOST_ADDR

#add net
route add default gw $ROUTE_ADDR
route add -net 14.2.0.0 netmask 255.255.0.0 dev br0
route del -net 14.0.0.0/8

#host machine do IP forwarding
#echo 1 > /proc/sys/net/ipv4/ip_forward

#display final result
/sbin/ifconfig -a
