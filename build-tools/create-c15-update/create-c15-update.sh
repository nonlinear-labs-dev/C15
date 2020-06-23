#!/bin/bash
#
# Author:       Anton Schmied
# Date:         12.02.2020
# vom Cmake übergebene Pfade zu den .tarS

EPC_UPDATE=$1
BBB_UPDATE=$2
LPC_UPDATE=$3
BINARY_DIR=$4
SOURCE_DIR=$5/build-tools/create-c15-update
OUTNAME=$6
ASPECTS=$7
OUT_DIRECTORY=$BINARY_DIR/$OUTNAME
OUT_TAR=$BINARY_DIR/$OUTNAME.tar

UPDATE_BBB=0
UPDATE_LPC=0
UPDATE_EPC=0

if [[ $ASPECTS = *epc* ]]
then
    UPDATE_EPC=1
fi

if [[ $ASPECTS = *lpc* ]]
then
    UPDATE_LPC=1
fi

if [[ $ASPECTS = *bbb* ]]
then
    UPDATE_BBB=1
fi

fail_and_exit() {
    echo "Failed to created an update!"
    exit 1
}

check_preconditions () {
    if [ -z "$EPC_UPDATE" -a $UPDATE_EPC == 1 ]; then echo "ePC update missing..." && return 1; fi
    if [ -z "$BBB_UPDATE" -a $UPDATE_BBB == 1 ]; then echo "BBB update missing..." && return 1; fi
    if [ -z "$LPC_UPDATE" -a $UPDATE_LPC == 1 ]; then echo "LPC update missing..." && return 1; fi
    return 0
}

create_update_file_structure() {
    echo "Creating Update Structure..."
    rm -rf $OUT_DIRECTORY
    mkdir $OUT_DIRECTORY || fail_and_exit
    if [ $UPDATE_BBB == 1 ]; then mkdir $OUT_DIRECTORY/BBB || fail_and_exit; fi
    if [ $UPDATE_EPC == 1 ]; then mkdir $OUT_DIRECTORY/EPC || fail_and_exit; fi
    if [ $UPDATE_LPC == 1 ]; then mkdir $OUT_DIRECTORY/LPC || fail_and_exit; fi
    mkdir $OUT_DIRECTORY/utilities || fail_and_exit
    echo "Creating Update Structure done."
    return 0
}

deploy_updates() {
    echo "Deploying updates..."

    if [ $UPDATE_BBB == 1 ]; then
        cp $BBB_UPDATE $OUT_DIRECTORY/BBB/ && chmod 666 $OUT_DIRECTORY/BBB/rootfs.tar.gz || fail_and_exit;
    fi

    if [ $UPDATE_EPC == 1 ]; then
        cp $EPC_UPDATE $OUT_DIRECTORY/EPC/update.tar && chmod 666 $OUT_DIRECTORY/EPC/update.tar || fail_and_exit;
    fi

    if [ $UPDATE_LPC == 1 ]; then
        cp $LPC_UPDATE $OUT_DIRECTORY/LPC/main.bin && chmod 666 $OUT_DIRECTORY/LPC/main.bin || fail_and_exit;
    fi

    echo "Deploying updates done."
    return 0
}

deploy_scripts() {
    echo "Deploying update scripts..."

    cp $SOURCE_DIR/update_scripts/run.sh $OUT_DIRECTORY/ && chmod 777 $OUT_DIRECTORY/run.sh || fail_and_exit;
    sed -i "s/ASPECTS=\"TO_BE_REPLACED_BY_CREATE_C15_UPDATE\"/ASPECTS=\"$ASPECTS\"/g" $OUT_DIRECTORY/run.sh

    if [ $UPDATE_BBB == 1 ]; then
        cp $SOURCE_DIR/update_scripts/bbb_update.sh $OUT_DIRECTORY/BBB/ && \
            chmod 777 $OUT_DIRECTORY/BBB/bbb_update.sh || \
            fail_and_exit;
    fi

    if [ $UPDATE_EPC == 1 ]; then
        cp $SOURCE_DIR/update_scripts/epc_pull_update.sh $OUT_DIRECTORY/EPC/ && \
            chmod 777 $OUT_DIRECTORY/EPC/epc_pull_update.sh && \
            cp $SOURCE_DIR/update_scripts/epc_push_update.sh $OUT_DIRECTORY/EPC/ && \
            chmod 777 $OUT_DIRECTORY/EPC/epc_push_update.sh && \
            cp $SOURCE_DIR/update_scripts/epc_fix.sh $OUT_DIRECTORY/EPC/ && \
            chmod 777 $OUT_DIRECTORY/EPC/epc_fix.sh || \
            fail_and_exit;
    fi

    if [ $UPDATE_LPC == 1 ]; then
        cp $SOURCE_DIR/update_scripts/lpc_update.sh $OUT_DIRECTORY/LPC/ && \
            chmod 777 $OUT_DIRECTORY/LPC/lpc_update.sh && \
            cp $SOURCE_DIR/update_scripts/lpc_check.sh $OUT_DIRECTORY/LPC/ && \
            chmod 777 $OUT_DIRECTORY/LPC/lpc_check.sh || \
            fail_and_exit;
    fi

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
}

get_tools_from_rootfs() {
    echo "Getting tools from rootfs..."
    rm -rf $BINARY_DIR/build-tools/bbb/rootfs
    mkdir $BINARY_DIR/build-tools/bbb/rootfs && tar -xf $BBB_UPDATE --exclude=./dev/* -C $BINARY_DIR/build-tools/bbb/rootfs

    for i in sshpass text2soled rsync socat thttpd lpc; do
        if ! cp $(find $BINARY_DIR/build-tools/bbb/rootfs/usr -type f -name "$i") $OUT_DIRECTORY/utilities/; then
          echo "could not get $i from rootfs"
          return 1
        fi
    done

    # once mxli is part of /usr/bin this can be done in the loop above
    if ! cp $(find $BINARY_DIR/build-tools/bbb/rootfs/ -type f -name "mxli") $OUT_DIRECTORY/utilities/; then
      echo "could not get mxli from rootfs"
      return 1
    fi

    for i in sshpass text2soled rsync socat thttpd mxli lpc; do
        if ! chmod +x $OUT_DIRECTORY/utilities/"$i"; then
          echo "could not make $i executable"
          return 1
        fi
    done

    for lib in libpopt.so.0.0.0 libpopt.so.0 libpopt.so; do
        if ! cp $(find $BINARY_DIR/build-tools/bbb/rootfs/lib/ -name "$lib") $OUT_DIRECTORY/utilities/; then
               echo "could not get library $lib from rootfs"
          return 1
        fi
    done

    cp -R $BINARY_DIR/build-tools/bbb/rootfs/usr/C15/text2soled/resources $OUT_DIRECTORY/utilities/

    echo "Getting tools from rootfs done."
    return 0
}


create_update_tar () {
    echo "Creating $OUT_TAR..."
    rm -rf $OUT_DIRECTORY/../nonlinear-c15-update.tar
    if cd $OUT_DIRECTORY/ && tar -cf $OUT_TAR * ; then
        echo "Creating $OUT_TAR done."
        return 0
    fi
    echo "Creating $OUT_TAR failed."
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

print_build_info()
{
    if [ ! -z "$1" ]; then
        echo -e "\e[33m$1\e[39m"
        echo "$(cat $1 | tr '\0' '\n' | grep --binary-files=text '~C15')"
    fi
}

print_epc_build_info()
{
    FILE=$BINARY_DIR/build-tools/epc/tmp/usr/local/C15/$1
    rm -f $FILE 2> /dev/null
    tar -C $BINARY_DIR/build-tools/epc/tmp --extract --file=$BINARY_DIR/build-tools/epc/tmp/update/NonLinuxOverlay.tar.gz ./usr/local/C15/$1
    print_build_info $FILE
}

print_C15_build_info() {
    echo "Getting build info..."
    if [ $UPDATE_EPC == 1 ]; then
        tar -C $BINARY_DIR/build-tools/epc/tmp --extract --file=$BINARY_DIR/build-tools/epc/update.tar ./update/NonLinuxOverlay.tar.gz
        print_epc_build_info  playground/playground
        print_epc_build_info  audio-engine/audio-engine
    fi
    if [ $UPDATE_LPC == 1 ]; then
        print_build_info $(find $BINARY_DIR/build-tools/lpc/ -type f -name "main.bin")
    fi
    if [ $UPDATE_BBB == 1 ]; then
        print_build_info $(find $BINARY_DIR/build-tools/bbb/rootfs/usr -type f -name "lpc")
        print_build_info $(find $BINARY_DIR/build-tools/bbb/rootfs/usr -type f -name "lpc-read")
        print_build_info $(find $BINARY_DIR/build-tools/bbb/rootfs/usr -type f -name "ehc")
        print_build_info $(find $BINARY_DIR/build-tools/bbb/rootfs/usr -type f -name "ehc-preset")
    fi
    echo "Getting build info done."
}

main() {
    check_preconditions || fail_and_exit
    create_update_file_structure || fail_and_exit
    deploy_updates || fail_and_exit
    deploy_scripts || fail_and_exit
    get_tools_from_rootfs || fail_and_exit
    create_update_tar || fail_and_exit
    print_C15_build_info
}

main
