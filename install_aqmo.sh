#!/bin/bash
make
sudo rmmod aqmo
sudo rm -rf /dev/aqmo

sudo insmod aqmo.ko || exit 1

major=$(awk "\$2 == \"aqmodev\" {print \$1}" /proc/devices)
echo "aqmo_major: ${major}"
sudo mknod /dev/aqmo c $major 0
sudo chmod 664 /dev/aqmo
