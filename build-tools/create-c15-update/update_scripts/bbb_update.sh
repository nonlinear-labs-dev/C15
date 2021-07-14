#!/bin/sh

# version : 2.0
#
# ----------- install a BBB Updates from USB-Stick ---------
#
# This script will transfer the /preset-manager/* and settings.xml to the ePC
# and update the BBB excluding the /update directory
#

EPC_IP=$1
BBB_IP=$2
OLD_MACHINE_ID=$(cat /etc/machine-id)

report_and_quit(){
    printf "$1" >> /update/errors.log
    exit $2
}

executeAsRoot() {
    echo "sscl" | /update/utilities/sshpass -p 'sscl' ssh -o ServerAliveInterval=1 -o ConnectionAttempts=1 -o ConnectTimeout=1 -o StrictHostKeyChecking=no sscl@$EPC_IP \
        "sudo -S /bin/bash -c '$1' 1>&2 > /dev/null"
    return $?
}

check_preconditions(){
    if [ ! -z "$EPC_IP" ] &&
        ping -c1 $EPC_IP 1>&2 > /dev/null &&
        executeAsRoot "exit" &&
        executeAsRoot "mountpoint -q /persistent"; then
        return 0
     fi
     return 1
}

move_files(){
    if [ -e /settings.xml ] && ! check_preconditions; then
        report_and_quit "E59 BBB update: Files on BBB present, but conditions are bad ..." "59"
    fi

    executeAsRoot "systemctl stop playground"

    if [ -d /internalstorage/preset-manager ] && [ "$(ls -A /internalstorage/preset-manager/)" ]; then
        executeAsRoot "scp -r root@$BBB_IP:/internalstorage/preset-manager/ /persistent" \
        && rm -rf /internalstorage/preset-manager/* \
        && rm -rf /internalstorage/preset-manager
        if [ $? -ne 0 ]; then report_and_quit "E55 BBB update: Moving presets to ePC failed ..." "55"; fi
    fi

    if [ -e /settings.xml ]; then
        executeAsRoot "scp root@$BBB_IP:/settings.xml /persistent/settings.xml" \
        && rm /settings.xml
        if [ $? -ne 0 ]; then report_and_quit "E56 BBB update: Moving settings to ePC failed ..." "56"; fi
    fi

    if [ -d /internalstorage/calibration ] && [ "$(ls -A /internalstorage/calibration/)" ]; then
        executeAsRoot "scp -r root@$BBB_IP:/internalstorage/calibration/ /persistent" \
        && rm -rf /internalstorage/calibration/* \
        && rm -rf /internalstorage/calibration
        if [ $? -ne 0 ]; then report_and_quit "E57 BBB update: Moving calibration settings to ePC failed ..." "57"; fi
    fi

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
    
    if [ ! -z "$EPC_IP" ]; then
        move_files
    fi
    update

    return 0
}

main



