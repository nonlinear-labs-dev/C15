#!/bin/sh

# version : 2.0
#
# ----------- install a ePC Updates from USB-Stick ---------
#
# This scripts will serve the update.tar via thttpd on the BBB and
# restart the ePC, which will evoke the Update

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
    killall thttpd

    if ! /update/utilities/thttpd -p 8000 -d /update/EPC/ -l /update/EPC/server.log; then
        report_and_quit "E46 ePC update: Could not start http server on BBB..." "46";
    fi

    for RETRYCOUNTER in {1..5}; do
        echo "restarting epc (try $RETRYCOUNTER)"
        executeAsRoot "reboot"

        if ! wait4playground; then
            report_and_quit "E45 ePC update: Reboot taking too long... timed out" "45"
        fi

        if cat /update/EPC/server.log | grep '"GET /update.tar HTTP/1.1" 200'; then
            rm /update/EPC/server.log
            killall thttpd
            return 0
        fi
    done

    killall thttpd
    rm /update/EPC/server.log
    report_and_quit "E47 ePC update: pulling update failed ..." "47"
    return 0
}

save_and_move_files(){
    mkdir /mnt/usb-stick/C15_user_files
    executeAsRoot "systemctl stop playground"

    if [ -d /internalstorage/preset-manager ] && [ "$(ls -A /internalstorage/preset-manager/)" ]; then
        cp -r /internalstorage/preset-manager/ /mnt/usb-stick/C15_user_files/ \
        && executeAsRoot "scp -r root@$BBB_IP:/internalstorage/preset-manager/ /persistent" \
        && rm -rf /internalstorage/preset-manager/* \
        && rm -rf /internalstorage/preset-manager
        if [ $? -ne 0 ]; then report_and_quit "E55 BBB update: Moving presets to ePC failed ..." "55"; fi
    fi

    if [ -e /settings.xml ]; then
        cp /settings.xml /mnt/usb-stick/C15_user_files/ \
        && executeAsRoot "scp root@$BBB_IP:/settings.xml /persistent/settings.xml" \
        && rm /settings.xml
        if [ $? -ne 0 ]; then report_and_quit "E56 BBB update: Moving settings to ePC failed ..." "56"; fi
    fi

    if [ -d /internalstorage/calibration ] && [ "$(ls -A /internalstorage/calibration/)" ]; then
        cp -r /internalstorage/calibration/ /mnt/usb-stick/C15_user_files/ \
        && executeAsRoot "scp -r root@$BBB_IP:/internalstorage/calibration/ /persistent" \
        && rm -rf /internalstorage/calibration/* \
        && rm -rf /internalstorage/calibration
        if [ $? -ne 0 ]; then report_and_quit "E57 BBB update: Moving calibration settings to ePC failed ..." "57"; fi
    fi

    if [ "$(ls -A /mnt/usb-stick/C15_user_files/)" ]; then
        cd /mnt/usb-stick/C15_user_files &&
        tar -cvf ../C15_user_files.tar * &&
        cd ~
    fi
    rm -rf /mnt/usb-stick/C15_user_files

    return 0
}

main () {
    update
    save_and_move_files
    return 0
}

main
