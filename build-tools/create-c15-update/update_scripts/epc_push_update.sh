#!/bin/bash

# version : 2.0
#
# ----------- install a ePC Updates from USB-Stick ---------
#
# This scripts will push the ePC update (os-overlay content) dierectly to /dev/sda3

# timeout is so long because the initramFS will potentialy be rebuild which takes quite a while

EPC_IP=$1
TIMEOUT=300

report_and_quit(){
    printf "$1" >> /update/errors.log
    exit $2
}

executeAsRoot() {
    echo "sscl" | /update/utilities/sshpass -p 'sscl' ssh -o ServerAliveInterval=1 -o ConnectionAttempts=1 -o ConnectTimeout=1 -o StrictHostKeyChecking=no sscl@$EPC_IP "sudo -S /bin/bash -c '$1' "
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

check_preconditions(){
    [ -z "$EPC_IP" ] && report_and_quit "E81: Usage: $EPC_IP <IP-of-ePC> wrong ..." "81"
    ping -c1 $EPC_IP 1>&2 > /dev/null || report_and_quit "E82: Can't ping ePC on $EPC_IP ..." "82"
    executeAsRoot "exit" || report_and_quit "E83: Can't logon to ePC OS ..." "83"
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

main () {
    check_preconditions
    update

    executeAsRoot "systemctl --force --force reboot"
    if ! wait4playground; then
        report_and_quit "E45: ePC update: Reboot taking too long... timed out" "45"
    fi
    return 0
}

main
