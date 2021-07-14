#!/bin/sh

echo "disabling AP"
systemctl stop accesspoint
echo $?
systemctl disable accesspoint
echo $?
systemctl mask accesspoint
echo $?