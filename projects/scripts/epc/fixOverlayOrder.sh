#!/bin/sh

if mount | grep "/lroot:/nloverlay/os-overlay"; then
    mkdir /tmp/sda2
    mount /dev/sda2 /tmp/sda2
    sed -i 's@/lroot:/nloverlay/os-overlay@/nloverlay/os-overlay:/lroot@' /tmp/sda2/lib/initcpio/hooks/oroot
    umount /tmp/sda2
    mkinitcpio -p linux
    mkinitcpio -p linux-rt
    reboot
fi