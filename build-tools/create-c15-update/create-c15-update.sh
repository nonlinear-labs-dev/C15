#!/bin/sh
#
# Author:       Anton Scmied
# Date:         12.02.2020
# TODO:         - deploy lpc update

# vom Cmake übergebene Pfade zu den .tarS
EPC_UPDATE=$1
BBB_UPDATE=$2
LPC_CORE_0=$3
LPC_CORE_1=$4
BINARY_DIR=$5
SOURCE_DIR=$6/create-c15-update
OUT_DIRECTORY=$BINARY_DIR/nonlinear-c15-update



fail_and_exit() {
    echo "Failed to created an update!"
    exit 1
}

check_preconditions () {
    if [ -z "$EPC_UPDATE" ]; then echo "ePC update missing..." && return 1; fi
    if [ -z "$BBB_UPDATE" ]; then echo "BBB update missing..." && return 1; fi
    if [ -z "$LPC_CORE_0" ]; then echo "LPC update missing..." && return 1; fi
    if [ -z "$LPC_CORE_1" ]; then echo "LPC update missing..." && return 1; fi
    return 0
}

create_update_file_structure() {
    echo "Creating Update Structure..."
    if mkdir -p $OUT_DIRECTORY \
        && mkdir -p $OUT_DIRECTORY/BBB \
        && mkdir -p $OUT_DIRECTORY/EPC \
        && mkdir -p $OUT_DIRECTORY/LPC \
        && mkdir -p $OUT_DIRECTORY/utilities; then
        echo "Creating Update Structure done."
        return 0
    fi
    echo "Creating Update Structure failed."
    return 1
}

deploy_updates() {
    echo "Deploying updates..."
    if cp $EPC_UPDATE $OUT_DIRECTORY/EPC/update.tar \
        && chmod 666 $OUT_DIRECTORY/EPC/update.tar \
        && cp $BBB_UPDATE $OUT_DIRECTORY/BBB/ \
        && chmod 666 $OUT_DIRECTORY/BBB/rootfs.tar.gz \
        && cp $LPC_CORE_0 $OUT_DIRECTORY/LPC/M0_project.bin \
        && chmod 666 $OUT_DIRECTORY/LPC/M0_project.bin \
        && cp $LPC_CORE_1 $OUT_DIRECTORY/LPC/M4_project.bin \
        && chmod 666 $OUT_DIRECTORY/LPC/M4_project.bin; then
        echo "Deploying updates done."
        return 0
     fi
     echo echo "Deploying updates failed."
     return 1
}

deploy_scripts() {
    echo "Deploying update scripts..."
    if cp $SOURCE_DIR/update_scripts/run.sh $OUT_DIRECTORY/ \
        && chmod 777 $OUT_DIRECTORY/run.sh \
        && cp $SOURCE_DIR/update_scripts/bbb_update.sh $OUT_DIRECTORY/BBB/ \
        && chmod 777 $OUT_DIRECTORY/BBB/bbb_update.sh \
        && cp $SOURCE_DIR/update_scripts/epc_pull_update.sh $OUT_DIRECTORY/EPC/ \
        && chmod 777 $OUT_DIRECTORY/EPC/epc_pull_update.sh \
        && cp $SOURCE_DIR/update_scripts/epc_push_update.sh $OUT_DIRECTORY/EPC/ \
        && chmod 777 $OUT_DIRECTORY/EPC/epc_push_update.sh \
        && cp $SOURCE_DIR/update_scripts/lpc_update.sh $OUT_DIRECTORY/LPC/ \
        && chmod 777 $OUT_DIRECTORY/LPC/lpc_update.sh; then
        echo "Deploying update scripts done."

        echo "Creating run.sh dummys..."
        if [ -e $OUT_DIRECTORY/run_v2.sh ] && [ -L $OUT_DIRECTORY/run_v2.sh ]; then
            rm -f $OUT_DIRECTORY/run_v2.sh
        fi
        if [ -e $OUT_DIRECTORY/run_v3.sh ] && [ -L $OUT_DIRECTORY/run_v3.sh ]; then
            rm -f $OUT_DIRECTORY/run_v3.sh
        fi
        touch $OUT_DIRECTORY/run_v2.sh && chmod 777 $OUT_DIRECTORY/run_v2.sh
        touch $OUT_DIRECTORY/run_v3.sh && chmod 777 $OUT_DIRECTORY/run_v3.sh
        echo "Creating run.sh dummys done"

        return 0
     fi
     echo echo "Deploying update scripts failed."
     return 1
}

get_tools_from_rootfs() {
    echo "Getting tools from rootfs..."
    mkdir -p $BINARY_DIR/rootfs && tar -xf $BBB_UPDATE -C $BINARY_DIR/rootfs

    for i in sshpass text2soled rsync socat thttpd; do
        if ! cp $(find $BINARY_DIR/rootfs/usr -type f -name "$i") $OUT_DIRECTORY/utilities/; then
          echo "could not get $i from rootfs"
          return 1
        fi
    done

    for lib in libpopt.so.0.0.0 libpopt.so.0 libpopt.so; do
        if ! cp $(find $BINARY_DIR/rootfs/lib/ -name "$lib") $OUT_DIRECTORY/utilities/; then
               echo "could not get library $lib from rootfs"
          return 1
        fi
    done

    cp -R $BINARY_DIR/rootfs/usr/C15/text2soled/resources $OUT_DIRECTORY/utilities/

    echo "Getting tools from rootfs done."
    return 0
}


create_update_tar () {
    echo "Creating nonlinear-c15-update.tar..."
    rm -rf $OUT_DIRECTORY/../nonlinear-c15-update.tar
    if cd $OUT_DIRECTORY/ && tar -cf ../nonlinear-c15-update.tar * ; then
        echo "Creating nonlinear-c15-update.tar done."
        return 0
    fi
    echo "Creating nonlinear-c15-update.tar failed."
    return 1
}


calc_checksum() {
    echo "Calc checksum..."
    if (cd $OUT_DIRECTORY/ && touch $(sha256sum ./nonlinear-c15-update.tar | grep -o "^[^ ]*").sign); then
        echo "Calc checksum done."
        return 0
    fi
    echo "Calc checksum failed."
    return 1
}


main() {
    check_preconditions || fail_and_exit
    create_update_file_structure || fail_and_exit
    deploy_updates || fail_and_exit
    deploy_scripts || fail_and_exit
    get_tools_from_rootfs || fail_and_exit
    create_update_tar || fail_and_exit
}

main
