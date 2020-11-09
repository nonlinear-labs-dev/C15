#!/bin/sh

# version : 2.0
#
# ----------- install a BBB Updates from USB-Stick ---------
#
# This script will transfer the /preset-manager/* and settings.xml to the ePC
# and update the BBB excluding the /update directory
#

OLD_MACHINE_ID=$(cat /etc/machine-id)

report_and_quit(){
    printf "$1" >> /update/errors.log
    exit $2
}

save_user_files_if_present(){
    mkdir /mnt/usb-stick/C15_user_files

    if [ -d /internalstorage/preset-manager ] && [ "$(ls -A /internalstorage/preset-manager/)" ]; then
        cp -r /internalstorage/preset-manager/ /mnt/usb-stick/C15_user_files/
    fi

    if [ -e /settings.xml ]; then
        cp /settings.xml /mnt/usb-stick/C15_user_files/
    fi

    if [ -d /internalstorage/calibration ] && [ "$(ls -A /internalstorage/calibration/)" ]; then
        cp -r /internalstorage/calibration/ /mnt/usb-stick/C15_user_files/
    fi

    if [ "$(ls -A /mnt/usb-stick/C15_user_files/)" ]; then
        cd /mnt/usb-stick/C15_user_files &&
        tar -cvf ../C15_user_files.tar * &&
        cd ~
    fi
    rm -rf /mnt/usb-stick/C15_user_files

    return 0
}

update(){
    systemctl status accesspoint
    ACCESSPOINT_RUNNING=$?

    mkdir /update/BBB/rootfs \
    && gzip -dc /update/BBB/rootfs.tar.gz | tar -C /update/BBB/rootfs -xf - \
    && LD_LIBRARY_PATH=/update/utilities /update/utilities/rsync -cax --exclude '/etc/hostapd.conf' --exclude '/var/log/journal' --exclude '/update' --delete /update/BBB/rootfs/ / \
    && chown -R root.root /update

    UPDATE_RESULT=$?

    if [ $ACCESSPOINT_RUNNING -eq 3 ]; then
        systemctl stop accesspoint
        systemctl disable accesspoint
        systemctl mask accesspoint
    else
        systemctl unmask accesspoint
        systemctl enable accesspoint
        systemctl start accesspoint
    fi

    if [ $UPDATE_RESULT -ne 0 ]; then report_and_quit "E58 BBB update: Syncing rootfs failed ..." "58"; fi
    mkdir /var/log/journal/$(cat /etc/machine-id)
    mv /var/log/journal/$OLD_MACHINE_ID/* /var/log/journal/$(cat /etc/machine-id)
    rm -rf /var/log/journal/$OLD_MACHINE_ID
    rm -rf /update/BBB/rootfs/*
    rm -rf /update/BBB/rootfs
}

main() {
    
    save_user_files_if_present
    update

    return 0
}

main



