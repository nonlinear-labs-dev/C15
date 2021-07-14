#!/bin/sh
# version : 2.0

freeze() {
    echo "Freezing from install-update-from-epc" >> /tmp/update.log
    while true; do
        sleep 1
    done
}

copy_from_epc() {
  sshpass -p 'sscl' ssh -o StrictHostKeyChecking=no sscl@192.168.10.10 'scp -o StrictHostKeyChecking=no /tmp/nonlinear-c15-update.tar root@192.168.10.11:/update'
}

unpack_update() {
    if rm -rf /update \
        && mkdir -p /update \
        && copy_from_epc \
        && cd /update \
        && tar xf nonlinear-c15-update.tar; then
            return 0
     fi
     return 1
}

check_preconditions() {
    if ! ls -l /update/BBB/ | grep bbb_update_1811a.sh \
        && ! ls -l /update/BBB/ | grep bbb_update_1905a.sh \
        && ! ls -l /update/BBB/ | grep playground-RELEASE-2019-05-1.5-706569a; then
            return 0
    fi
    rm -rf /update/*
    return 1
}

run_update() {
    chmod +x /update/run.sh
    /bin/sh /update/run.sh
    freeze
}


main() {
    echo "install-update-from-epc.sh" >> /tmp/update.log
    unpack_update && check_preconditions && run_update

    return 0
}

main
