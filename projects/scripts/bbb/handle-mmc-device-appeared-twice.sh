#!/bin/sh

LOGFILE=/HANDLED_MMC_DEVICE_APPEARED_TWICE.log      # log all detected failures
STAMPFILE=/HANDLED_MMC_DEVICE_APPEARED_TWICE.stamp  # did the error happen on current boot?

detectError() {
    journalctl -b0 | grep "appeared twice with different sysfs paths" > /dev/null
    return $?
}

doesStampFileExist() {
    test -f $STAMPFILE
    return $?
}

isConscutiveError() {
    doesStampFileExist
    return $?
}

removeStampFile() {
    rm -f $STAMPFILE
    return 0
}

logErrorAndReboot() {
    date >> $LOGFILE
    touch $STAMPFILE
    reboot now
    return 0
}

handleError() {
    if isConscutiveError; then
        echo "Will not trigger reboot to avoid endless boot loop."
        removeStampFile
    else
        echo "Will trigger reboot."
        logErrorAndReboot
    fi
}

main() {
    sleep 2

    if detectError; then
        echo "MMC/sysfs error detected."
        handleError
    else
        echo "No MMC/sysfs error detected."
        removeStampFile
    fi
}

main
