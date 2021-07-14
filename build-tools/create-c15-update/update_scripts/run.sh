#!/bin/sh
# version : 2.0

EPC_IP=192.168.10.10
BBB_IP=192.168.10.11

# general Messages
MSG_DO_NOT_SWITCH_OFF="DO NOT SWITCH OFF!"
MSG_STARTING_UPDATE="Starting the C15 update..."
MSG_UPDATING_C15="Updating the C15"
MSG_UPDATING_EPC="1/3 Updating..."
MSG_UPDATING_BBB="2/3 Updating..."
MSG_UPDATING_RT_FIRMWARE="3/3 Updating..."
MSG_DONE="DONE!"
MSG_FAILED="FAILED!"
MSG_FAILED_WITH_ERROR_CODE="FAILED! Error code:"
MSG_CHECK_LOG="Please check update log!"
MSG_RESTART_MAN="Please restart!"
MSG_RESTART_AUT="Will restart now!"

ASPECTS="TO_BE_REPLACED_BY_CREATE_C15_UPDATE"

UPDATE_BBB=0
UPDATE_PLAYCONTROLLER=0
UPDATE_EPC=0

if [[ $ASPECTS = *epc* ]]
then
    UPDATE_EPC=1
    echo "will update epc"
fi

if [[ $ASPECTS = *playcontroller* ]]
then
    UPDATE_PLAYCONTROLLER=1
    echo "will update playcontroller"
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

freeze() {
    while true; do
        sleep 1
    done
}

determine_termination() {
    cat /nonlinear/scripts/install-update.sh | grep 'version : 1.0' \
        && freeze || exit 1
}

configure_ssh() {
    echo "Host 192.168.10.10
            StrictHostKeyChecking no
            UserKnownHostsFile=/dev/null
            " > ~/.ssh/config
    chmod 400 ~/.ssh/config
}

report() {
    pretty "$1" "$2" "$3" "$2" "$3"
    printf "$2" >> /update/errors.log
    echo "$2" | systemd-cat -t "C15_Update"
}

executeAsRoot() {
    echo "sscl" | /update/utilities/sshpass -p 'sscl' ssh -o ServerAliveInterval=1 -o ConnectionAttempts=1 -o ConnectTimeout=1 -o StrictHostKeyChecking=no sscl@$EPC_IP "sudo -S /bin/bash -c '$1'"
    return $?
}

executeOnWin() {
    /update/utilities/sshpass -p 'TEST' ssh -o ConnectionAttempts=1 -o ConnectTimeout=1 -o StrictHostKeyChecking=no TEST@$EPC_IP "$1" 1>&2 > /dev/null
    return $?
}

wait4epc() {
    TIMEOUT=$1
    for COUNTER in $(seq 1 $TIMEOUT); do
        echo "waiting for OS response ... $COUNTER/$TIMEOUT"
        sleep 1
        executeAsRoot "exit" && return 0
    done
    return 1
}

check_preconditions() {
    [ $UPDATE_EPC == 0 ] && return 0
    if ! wait4epc 10; then
        if [ -z "$EPC_IP" ]; then report "" "E81: Usage: $EPC_IP <IP-of-ePC> wrong ..." "Please retry update!" && return 1; fi
        if ! ping -c1 $EPC_IP 1>&2 > /dev/null; then  report "" "E82: Cannot ping ePC on $EPC_IP ..." "Please retry update!" && return 1; fi
        if executeOnWin "mountvol p: /s & p: & DIR P:\nonlinear"; then
            report "" "E84: Upgrade OS first!" "Please contact NLL!" && return 1
        else
            if mount.cifs //192.168.10.10/update /mnt/windows -o user=TEST,password=TEST \
                && ls -l /mnt/windows/ | grep Phase22Renderer.ens; then
                    report "" "E85: OS too old for update!" "Please contact NLL!" && return 1
            fi
        fi
        report "" "Something went wrong!" "Please retry update!" && return 1
    fi
    
    rm /update/EPC/update.tar

    if [[ "$(executeAsRoot "uname -r")" == "4.9.9-rt6-1-rt" ]]; then
        [ -f /update/EPC/update_5-7i3.tar ] && { ln -s /update/EPC/update_5-7i3.tar /update/EPC/update.tar; FIX_EPC=true; } ||
            { report "" "E86: ePC update missing" "Please retry download!"; return 1; }
    else
        [ -f /update/EPC/update_10i3.tar ] && { ln -s /update/EPC/update_10i3.tar /update/EPC/update.tar; FIX_EPC=false; } ||
            { report "" "E86: ePC update missing" "Please retry download!"; return 1; }
    fi

    if $UPDATE_BBB == 1; then
      [ -f "/update/BBB/rootfs.tar.gz" ] || { report "" "E87: BBB update missing" "Please retry download!"; return 1; }
    fi
    
    if $UPDATE_PLAYCONTROLLER == 1; then
      [ -f "/update/playcontroller/main.bin" ] || { report "" "E88: playcontroller update missing" "Please retry download!"; return 1; }
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
    return $?
}

epc_update() {
    pretty "" "$MSG_UPDATING_EPC" "$MSG_DO_NOT_SWITCH_OFF" "$MSG_UPDATING_EPC" "$MSG_DO_NOT_SWITCH_OFF"

    if ! epc_push_update; then
        epc_pull_update
        return_code=$?
        if [ $return_code -ne 0 ]; then
            pretty "" "$MSG_UPDATING_EPC" "$MSG_FAILED_WITH_ERROR_CODE $return_code" "$MSG_UPDATING_EPC" "$MSG_FAILED_WITH_ERROR_CODE $return_code"
            sleep 2
            return 1
        fi
    fi

    if [[ "$FIX_EPC" == "true" ]]; then
        epc_fix
        return_code=$?
        if [ $return_code -ne 0 ]; then
            /update/utilities/sshpass -p "sscl" scp -r sscl@192.168.10.10:/tmp/fix_error.log /dev/stdout | cat - >> /update/errors.log
            pretty "" "$MSG_UPDATING_EPC" "$MSG_FAILED_WITH_ERROR_CODE $return_code" "$MSG_UPDATING_EPC" "$MSG_FAILED_WITH_ERROR_CODE $return_code"
            sleep 2
            return 1
        fi
        executeAsRoot "reboot"
        wait4epc 60
    fi

    pretty "" "$MSG_UPDATING_EPC" "$MSG_DONE" "$MSG_UPDATING_EPC" "$MSG_DONE"
    sleep 2
    return 0
}

bbb_update() {
    pretty "" "$MSG_UPDATING_BBB" "$MSG_DO_NOT_SWITCH_OFF" "$MSG_UPDATING_BBB" "$MSG_DO_NOT_SWITCH_OFF"
    chmod +x /update/BBB/bbb_update.sh
    if [ $UPDATE_EPC -eq 1 ]; then
        /bin/sh /update/BBB/bbb_update.sh $EPC_IP $BBB_IP
    else
        /bin/sh /update/BBB/bbb_update.sh
    fi

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

playcontroller_update() {
    pretty "" "$MSG_UPDATING_RT_FIRMWARE" "$MSG_DO_NOT_SWITCH_OFF" "$MSG_UPDATING_RT_FIRMWARE" "$MSG_DO_NOT_SWITCH_OFF"
    chmod +x /update/playcontroller/playcontroller_update.sh
    chmod +x /update/playcontroller/playcontroller_check.sh
    rm -f /update/mxli.log

    /bin/sh /update/playcontroller/playcontroller_update.sh /update/playcontroller/main.bin A && sleep 1 && /bin/sh /update/playcontroller/playcontroller_check.sh 5

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
    rm -f /mnt/usb-stick/nonlinear-c15-update.log.txt
    rm -f /update/errors.log
    touch /update/errors.log
    chmod +x /update/utilities/*

    configure_ssh
    check_preconditions || determine_termination

    pretty "" "$MSG_STARTING_UPDATE" "$MSG_DO_NOT_SWITCH_OFF" "$MSG_STARTING_UPDATE" "$MSG_DO_NOT_SWITCH_OFF"
    sleep 2

    stop_services
    [ $UPDATE_EPC == 1 ] && epc_update
    [ $UPDATE_BBB == 1 ] && bbb_update
    [ $UPDATE_PLAYCONTROLLER == 1 ] && playcontroller_update

    if [ $(wc -c /update/errors.log | awk '{print $1}') -ne 0 ]; then
        cp /update/errors.log /mnt/usb-stick/nonlinear-c15-update.log.txt
        pretty "" "$MSG_UPDATING_C15 $MSG_FAILED" "$MSG_CHECK_LOG" "$MSG_UPDATING_C15 $MSG_FAILED" "$MSG_CHECK_LOG"
        determine_termination
    fi

    if [ "$1" = "reboot" ]; then
        pretty "" "$MSG_UPDATING_C15 $MSG_DONE" "$MSG_RESTART_AUT" "$MSG_UPDATING_C15 $MSG_DONE" "$MSG_RESTART_AUT"
        sleep 2
        executeAsRoot "systemctl --force --force reboot"
        reboot
    fi

    pretty "" "$MSG_UPDATING_C15 $MSG_DONE" "$MSG_RESTART_MAN" "$MSG_UPDATING_C15 $MSG_DONE" "$MSG_RESTART_MAN"
    return 0
}

main $1
