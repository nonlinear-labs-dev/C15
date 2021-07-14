#!/bin/sh

echo "enabling and starting AP"
systemctl unmask accesspoint
echo $?
systemctl enable accesspoint
echo $?
systemctl start accesspoint
echo $?