#!/bin/sh
# version : 2.0

freeze() {
    while true; do
        sleep 1
    done
}

unpack_update() {
    if rm -rf /update \
        && mkdir -p /update \
        && cp /mnt/usb-stick/nonlinear-c15-update.tar /update \
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
    /bin/sh /update/run.sh && mv /mnt/usb-stick/nonlinear-c15-update.tar /mnt/usb-stick/nonlinear-c15-update.tar-copied
    freeze
}


main() {
    if [ -e /mnt/usb-stick/nonlinear-c15-update.tar ]; then
        unpack_update && check_preconditions && run_update
    fi

    return 0
}

main
