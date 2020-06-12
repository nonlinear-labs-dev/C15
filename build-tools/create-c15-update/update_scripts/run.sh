#!/bin/sh

EPC_IP=192.168.10.10
BBB_IP=192.168.10.11

# general Messages
MSG_DO_NOT_SWITCH_OFF="DO NOT SWITCH OFF C15!"
MSG_STARTING_UPDATE="Starting C15 update..."
MSG_UPDATING_C15="Updating C15"
MSG_UPDATING_EPC="1/3 Updating..."
MSG_UPDATING_BBB="2/3 Updating..."
MSG_UPDATING_RT_FIRMWARE="3/3 Updating..."
MSG_DONE="DONE!"
MSG_FAILED="FAILED!"
MSG_FAILED_WITH_ERROR_CODE="FAILED! Error Code:"
MSG_CHECK_LOG="Please check update log!"
MSG_RESTART="Please Restart!"

ASPECTS="TO_BE_REPLACED_BY_CREATE_C15_UPDATE"

UPDATE_BBB=0
UPDATE_LPC=0
UPDATE_EPC=0

if [[ $ASPECTS = *epc* ]]
then
    UPDATE_EPC=1
    echo "will update epc"
fi

if [[ $ASPECTS = *lpc* ]]
then
    UPDATE_LPC=1
    echo "will update lpc"
fi

if [[ $ASPECTS = *bbb* ]]
then
    UPDATE_BBB=1
    echo "will update bbb"
fi

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

report() {
    pretty "$1" "$2" "$3" "$2" "$3"
    printf "$2" >> /update/errors.log
    echo "$2" | systemd-cat -t "C15_Update"
}

executeAsRoot() {
    echo "sscl" | /update/utilities/sshpass -p 'sscl' ssh -o ConnectionAttempts=1 -o ConnectTimeout=1 -o StrictHostKeyChecking=no sscl@$EPC_IP "sudo -S /bin/bash -c '$1' 1>&2 > /dev/null"
    return $?
}

executeOnWin() {
    /update/utilities/sshpass -p 'TEST' ssh -o ConnectionAttempts=1 -o ConnectTimeout=1 -o StrictHostKeyChecking=no TEST@$EPC_IP "$1" 1>&2 > /dev/null
    return $?
}

TIMEOUT=10

isNllLinuxSystem() {
    for COUNTER in $(seq 1 $TIMEOUT); do
        echo "checking for NLL linux ... $COUNTER/$TIMEOUT"
        sleep 1
        executeAsRoot "test -d /usr/local/C15" && echo "MS1.7++ found" && return 0
        executeAsRoot "test -d /usr/local/nonlinear" && echo "MS1.5 found" && return 0
    done
    return 1
}

check_preconditions() {
    if ! isNllLinuxSystem; then
        if [ -z "$EPC_IP" ]; then report "" "E81: Usage: $EPC_IP <IP-of-ePC> wrong ..." "Please retry update!" && return 1; fi
        if ! ping -c1 $EPC_IP 1>&2 > /dev/null; then  report "" "E82: Cannot ping ePC on $EPC_IP ..." "Please retry update!" && return 1; fi
        if executeOnWin "mountvol p: /s & p: & DIR P:\nonlinear"; then
            report "" "E84: Upgrade OS first!" "Please contact NLL!" && return 1
        else
            if mount.cifs //192.168.10.10/update /mnt/windows -o user=TEST,password=TEST \
                && ls -l /mnt/windows/ | grep Phase22Renderer.ens; then
                    report "" "E85: OS too old for update!" "Please contact NLL!" && return 1
            else
              report "" "E86: Can't detect OS!" "Please contact NLL!" && return 1
            fi
        fi
    fi
    return 0
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

epc_fix() {
    /update/utilities/sshpass -p "sscl" scp -r /update/EPC/epc_fix.sh sscl@192.168.10.10:/tmp
    executeAsRoot "cd /tmp && chmod +x epc_fix.sh && ./epc_fix.sh"
    result=$?

    if [ $result -ne 0 ]; then
        executeAsRoot "cat /tmp/fix_error.log" >> /update/errors.log && return $result
    fi
    return 0
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

    if ! epc_fix; then
        pretty "" "$MSG_UPDATING_EPC" "$MSG_FAILED_WITH_ERROR_CODE $return_code" "$MSG_UPDATING_EPC" "$MSG_FAILED_WITH_ERROR_CODE $return_code"
        sleep 2
        return 1
    fi

    pretty "" "$MSG_UPDATING_EPC" "$MSG_DONE" "$MSG_UPDATING_EPC" "$MSG_DONE"
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

    configure_ssh
    pretty "" "$MSG_UPDATING_BBB" "$MSG_DONE" "$MSG_UPDATING_BBB" "$MSG_DONE"

    sleep 2
    return 0
}

lpc_update() {
    pretty "" "$MSG_UPDATING_RT_FIRMWARE" "$MSG_DO_NOT_SWITCH_OFF" "$MSG_UPDATING_RT_FIRMWARE" "$MSG_DO_NOT_SWITCH_OFF"
    chmod +x /update/LPC/lpc_update.sh
    rm -f /update/mxli.log

    /bin/sh /update/LPC/lpc_update.sh /update/LPC/main.bin A

    # error codes 30...39
    return_code=$?
    if [ $return_code -ne 0 ]; then
        pretty "" "$MSG_UPDATING_RT_FIRMWARE" "$MSG_FAILED_WITH_ERROR_CODE $return_code" "$MSG_UPDATING_RT_FIRMWARE" "$MSG_FAILED_WITH_ERROR_CODE $return_code"
        sleep 2
        return 1;
    fi

    pretty "" "$MSG_UPDATING_RT_FIRMWARE" "$MSG_DONE" "$MSG_UPDATING_RT_FIRMWARE" "$MSG_DONE"
    sleep 2
    return 0
}

stop_services() {
    systemctl stop playground > /dev/null || executeAsRoot "systemctl stop playground"
    systemctl stop bbbb > /dev/null
    return 0
}

main() {
    rm -f /update/errors.log
    touch /update/errors.log

    configure_ssh
    check_preconditions || return 1

    pretty "" "$MSG_STARTING_UPDATE" "$MSG_DO_NOT_SWITCH_OFF" "$MSG_STARTING_UPDATE" "$MSG_DO_NOT_SWITCH_OFF"
    sleep 2

    stop_services
    [ $UPDATE_EPC == 1 ] && epc_update
    [ $UPDATE_BBB == 1 ] && bbb_update
    [ $UPDATE_LPC == 1 ] && lpc_update

    if [ $(wc -c /update/errors.log | awk '{print $1}') -ne 0 ]; then
        cp /update/errors.log /mnt/usb-stick/nonlinear-c15-update.log.txt
        pretty "" "$MSG_UPDATING_C15 $MSG_FAILED" "$MSG_CHECK_LOG" "$MSG_UPDATING_C15 $MSG_FAILED" "$MSG_CHECK_LOG"
        return 1
    fi

    pretty "" "$MSG_UPDATING_C15 $MSG_DONE" "$MSG_RESTART" "$MSG_UPDATING_C15 $MSG_DONE" "$MSG_RESTART"
    return 0
}

main
