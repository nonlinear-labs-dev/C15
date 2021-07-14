#!/bin/sh

echo "disabling AP"
systemctl stop accesspoint
systemctl disable accesspoint
systemctl mask accesspoint