#!/bin/bash

# version : 2.0
#
# ----------- install a ePC Updates from USB-Stick ---------
#
# This scripts will push the ePC update (os-overlay content) dierectly to /dev/sda3

# timeout is so long because the initramFS will potentialy be rebuild which takes quite a while

EPC_IP=$1
BBB_IP=$2
TIMEOUT=300

report_and_quit(){
    printf "$1" >> /update/errors.log
    exit $2
}

executeAsRoot() {
    echo "sscl" | /update/utilities/sshpass -p 'sscl' ssh -o ConnectionAttempts=1 -o ConnectTimeout=1 -o StrictHostKeyChecking=no sscl@$EPC_IP "sudo -S /bin/bash -c '$1' "
    return $?
}

wait4playground() {
    for COUNTER in $(seq 1 $TIMEOUT); do
        echo "awaiting reboot ... $COUNTER/$TIMEOUT"
        sleep 1
        executeAsRoot "systemctl status playground" && return 0
    done
    return 1
}

update(){
    /update/utilities/sshpass -p 'sscl' scp /update/EPC/update.tar sscl@$EPC_IP:/tmp

    executeAsRoot "systemctl stop audio-engine; systemctl stop playground"
    executeAsRoot "mkdir -p /tmp/sda3; mount /dev/sda3 /tmp/sda3"
    
    if executeAsRoot "rm -rf /tmp/sda3/os-overlay/*" &&
       executeAsRoot "rm -rf /tmp/sda3/update-scratch/*" &&
       executeAsRoot "tar -x -C /tmp/sda3/update-scratch -f /tmp/update.tar" &&
       executeAsRoot "tar -x -C /tmp/sda3/os-overlay -f /tmp/sda3/update-scratch/update/NonLinuxOverlay.tar.gz" &&
       executeAsRoot "umount /tmp/sda3"; then
       return 0
    fi

    report_and_quit "E49 ePC update: pushing update failed ..." "49"
}

move_files(){
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

main () {
    update
    executeAsRoot "reboot"
    if ! wait4playground; then
        report_and_quit "E45: ePC update: Reboot taking too long... timed out" "45"
    fi
    move_files
    return 0
}

main
