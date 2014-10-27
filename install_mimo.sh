#!/bin/bash
make
sudo rmmod mimo
sudo rm -rf /dev/mimo

sudo insmod mimo.ko || exit 1

major=$(awk "\$2 == \"mimodev\" {print \$1}" /proc/devices)
echo "mimo_major: ${major}"
sudo mknod /dev/mimo c $major 0
sudo chmod 664 /dev/mimo
