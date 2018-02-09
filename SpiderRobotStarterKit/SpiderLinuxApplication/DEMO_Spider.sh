#!/bin/sh
PATH=/home/root/
export PATH
/etc/init.d/dbus-1 start
bluetoothd 
./spider
exit 0
