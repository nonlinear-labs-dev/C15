#!/bin/sh

rm -rf /update
mkdir -p /update

if [ -e /mnt/usb-stick/nonlinear-c15-update.tar ]
then
	cp /mnt/usb-stick/nonlinear-c15-update.tar /update		
	cd /update
	tar xf nonlinear-c15-update.tar
	mv /mnt/usb-stick/nonlinear-c15-update.tar /mnt/usb-stick/nonlinear-c15-update.tar-copied
	chmod +x /update/run.sh
	/bin/sh /update/run.sh
fi

