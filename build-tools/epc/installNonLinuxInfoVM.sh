#!/bin/sh

qemu-img create -f vmdk /workdir/disk.vmdk 32G
touch serial.log
tail -f serial.log &
(sleep 5 && echo \"1\") | qemu-system-x86_64 -enable-kvm -chardev stdio,id=char0,logfile=serial.log,signal=off -serial chardev:char0 -display none -smp 8 -m 8G -hda /workdir/disk.vmdk -cdrom /workdir/NonLinuxAP.iso
chown $USER_ID.$GROUP_ID /workdir/disk.vmdk
