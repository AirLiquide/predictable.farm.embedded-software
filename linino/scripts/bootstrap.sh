#!bin/bash

source /root/valueID

#echo predictable_$valueID
hostname predictable$valueID
sed -i "s/option hostname.*/option hostname 'predictable$valueID'/g" /etc/config/system
sed -i "s/export ID=.*/export ID=$valueID/g" /etc/init.d/bridge

reboot
