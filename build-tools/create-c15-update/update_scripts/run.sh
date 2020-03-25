#!/bin/sh
#
#
# Name:         Anton Schmied
# Date:         2020.02.27
# Version:      2.0
# TODO:         Trouble Shooting if one of the updates does not work?
#               Save to journalctl
#               Where to save the update log, if update is not from USB-Stick?
#               -> under /persistent/ ?? or is bbb journalctl enough?

# ePC IP
EPC_IP=192.168.10.10
BBB_IP=192.168.10.11

# general Messages
MSG_DO_NOT_SWITCH_OFF="DO NOT SWITCH OFF C15!"
MSG_UPDATING_C15="updateing C15..."
MSG_UPDATING_RT_FIRMWARE_1="updating RT-System 1..."
MSG_UPDATING_RT_FIRMWARE_2="updating RT-System 2..."
MSG_UPDATING_EPC="updating ePC..."
MSG_UPDATING_BBB="updating BBB..."
MSG_DONE="DONE"
MSG_FAILED="FAILED"
MSG_FAILED_WITH_ERROR_CODE="FAILED! Error Code: "

# OBSOLETE??
MSG_DONE_WITH_WARNING_CODE="OK. Warning: "
#MSG_REBOOT="PLEASE RESTART C15 NOW"
#MSG_UPDATING_SYSTEM_FILES="updating system files..."
#MSG_CREATING_BACKUP="creating backup "
#MSG_UNINSTALLING="uninstalling "
#MSG_SAVING_PRESETS="saving presets..."
#MSG_RESTORING_PRESETS="restoring presets..."

t2s() {
    /update/utilities/text2soled multitext "$1" "$2" "$3" "$4" "$5" "$6"
}

pretty() {
    echo "$*"
    HEADLINE="$1"
    BOLED_LINE_1="$2"
    BOLED_LINE_2="$3"
    SOLED_LINE_2="$4"
    SOLED_LINE_3="$5"

    t2s "${HEADLINE}@b1c" "${BOLED_LINE_1}@b3c" "${BOLED_LINE_2}@b4c" "${SOLED_LINE_2}@s1c" "${SOLED_LINE_3}@s2c"
}

report_and_quit() {
    pretty "$1" "$2" "$3" "$4" "$5" "$6"
    echo "$2 $3" | systemd-cat -t "C15_Update"
#    exit 1
}

epc_push_update() {
    chmod +x /update/EPC/epc_push_update.sh
    /bin/sh /update/EPC/epc_push_update.sh $EPC_IP
    return $?
}

epc_pull_update() {
    chmod +x /update/EPC/epc_pull_update.sh
    /bin/sh /update/EPC/epc_pull_update.sh $EPC_IP
    return $?
}

epc_update() {
    pretty "" "$MSG_UPDATING_EPC" "$MSG_DO_NOT_SWITCH_OFF" "$MSG_UPDATING_EPC" "$MSG_DO_NOT_SWITCH_OFF"

    if ! epc_push_update; then
        if ! epc_pull_update; then
            pretty "" "$MSG_UPDATING_EPC" "$MSG_FAILED_WITH_ERROR_CODE $return_code" "$MSG_UPDATING_EPC" "$MSG_FAILED_WITH_ERROR_CODE $return_code"
            sleep 2
            return 1
        fi
    fi

    pretty "" "$MSG_UPDATING_EPC" "$MSG_DONE" "$MSG_UPDATING_EPC" "$MSG_DONE"
    sleep 2
    return 0
}


bbb_update() {
    pretty "" "$MSG_UPDATING_BBB" "$MSG_DO_NOT_SWITCH_OFF" "$MSG_UPDATING_BBB" "$MSG_DO_NOT_SWITCH_OFF"
    chmod +x /update/BBB/bbb_update.sh
    /bin/sh /update/BBB/bbb_update.sh $EPC_IP $BBB_IP

    # error codes 50...59
    return_code=$?
    if [ $return_code -ne 0 ]; then
        pretty "" "$MSG_UPDATING_BBB" "$MSG_FAILED_WITH_ERROR_CODE $return_code" "$MSG_UPDATING_BBB" "$MSG_FAILED_WITH_ERROR_CODE $return_code"
        sleep 2
        return 1;
    fi

    pretty "" "$MSG_UPDATING_BBB" "$MSG_DONE" "$MSG_UPDATING_BBB" "$MSG_DONE"
    sleep 2
    return 0
}

lpc_update() {
    pretty "" "$MSG_UPDATING_RT_FIRMWARE_1" "$MSG_DO_NOT_SWITCH_OFF" "$MSG_UPDATING_RT_FIRMWARE_1" "$MSG_DO_NOT_SWITCH_OFF"
    chmod +x /update/LPC/lpc_update.sh
    rm -f /update/mxli.log

    /bin/sh /update/LPC/lpc_update.sh /update/LPC/M0_project.bin B

    # error codes 30...39
    return_code=$?
    if [ $return_code -ne 0 ]; then
        pretty "" "$MSG_UPDATING_RT_FIRMWARE_1" "$MSG_FAILED_WITH_ERROR_CODE $return_code" "$MSG_UPDATING_RT_FIRMWARE_1" "$MSG_FAILED_WITH_ERROR_CODE $return_code"
        sleep 2
        return 1;
    fi

    pretty "" "$MSG_UPDATING_RT_FIRMWARE_1" "$MSG_DONE" "$MSG_UPDATING_RT_FIRMWARE_1" "$MSG_DONE"
    sleep 2

    pretty "" "$MSG_UPDATING_RT_FIRMWARE_2" "$MSG_DO_NOT_SWITCH_OFF" "$MSG_UPDATING_RT_FIRMWARE_2" "$MSG_DO_NOT_SWITCH_OFF"
    /bin/sh /update/LPC/lpc_update.sh /update/LPC/M4_project.bin A

    # error codes 30...39
    return_code=$?
    if [ $return_code -ne 0 ]; then
        pretty "" "$MSG_UPDATING_RT_FIRMWARE_2" "$MSG_FAILED_WITH_ERROR_CODE $return_code" "$MSG_UPDATING_RT_FIRMWARE_2" "$MSG_FAILED_WITH_ERROR_CODE $return_code"
        sleep 2
        return 1;
    fi

    pretty "" "$MSG_UPDATING_RT_FIRMWARE_2" "$MSG_DONE" "$MSG_UPDATING_RT_FIRMWARE_2" "$MSG_DONE"
    sleep 2
    return 0
}

configure_ssh() {
    echo "Host 192.168.10.10
            StrictHostKeyChecking no
            UserKnownHostsFile=/dev/null
            " > ~/.ssh/config
    chmod 400 ~/.ssh/config
}

stop_services() {
    systemctl stop playground > /dev/null
    systemctl stop bbbb > /dev/null
}

rebootEPC() {
    echo "sscl" | /update/utilities/sshpass -p 'sscl' ssh -o ConnectionAttempts=1 -o ConnectTimeout=1 -o StrictHostKeyChecking=no sscl@$EPC_IP "sudo reboot"
}

rebootBBB() {
    reboot
}

main() {
    rm -f /update/errors.log
    touch /update/errors.log

    configure_ssh
    stop_services

    epc_update
    bbb_update
    lpc_update

    if [ $(wc -c /update/errors.log | awk '{print $1}') -ne 0 ]; then
        cp /update/errors.log /mnt/usb-stick/nonlinear-c15-update.log.txt
        pretty "" "$MSG_UPDATING_C15" "$MSG_FAILED" "$MSG_UPDATING_C15" "$MSG_FAILED"
        sleep 2
        pretty "" "Check Update Log..." "" "Check Update Log..." ""
        sleep 2
    else
        pretty "" "$MSG_UPDATING_C15" "$MSG_DONE" "$MSG_UPDATING_C15" "$MSG_DONE"
        sleep 2
    fi

    pretty "" "Rebooting System..." "" "Rebooting System..." ""
    sleep 2

    rebootEPC
    rebootBBB
}

main
