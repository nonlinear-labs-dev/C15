#!/bin/bash
#
# Date:         12.02.2020
# vom Cmake übergebene Pfade zu den .tarS

EPC_UPDATE=$1
EPC_2_UPDATE=$2
BBB_UPDATE=$3
PLAYCONTROLLER_UPDATE=$4
BINARY_DIR=$5
SOURCE_DIR=$6/build-tools/create-c15-update
OUTNAME=$7
ASPECTS=$8
OUT_DIRECTORY=$BINARY_DIR/$OUTNAME
OUT_TAR=$BINARY_DIR/$OUTNAME.tar

UPDATE_BBB=0
UPDATE_PLAYCONTROLLER=0
UPDATE_EPC=0

if [[ $ASPECTS = *epc* ]]
then
    UPDATE_EPC=1
fi

if [[ $ASPECTS = *playcontroller* ]]
then
    UPDATE_PLAYCONTROLLER=1
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
    if [ -z "$PLAYCONTROLLER_UPDATE" -a $UPDATE_PLAYCONTROLLER == 1 ]; then echo "playcontroller update missing..." && return 1; fi
    return 0
}

create_update_file_structure() {
    echo "Creating Update Structure..."
    rm -rf $OUT_DIRECTORY
    mkdir $OUT_DIRECTORY || fail_and_exit
    if [ $UPDATE_BBB == 1 ]; then mkdir $OUT_DIRECTORY/BBB || fail_and_exit; fi
    if [ $UPDATE_EPC == 1 ]; then mkdir $OUT_DIRECTORY/EPC || fail_and_exit; fi
    if [ $UPDATE_PLAYCONTROLLER == 1 ]; then mkdir $OUT_DIRECTORY/playcontroller || fail_and_exit; fi
    mkdir $OUT_DIRECTORY/utilities || fail_and_exit
    echo "Creating Update Structure done."
    return 0
}

deploy_updates() {
    echo "Deploying updates..."

    if [ $UPDATE_BBB == 1 ]; then
        echo "Will deploy BBB update."
        cp $BBB_UPDATE $OUT_DIRECTORY/BBB/ && chmod 666 $OUT_DIRECTORY/BBB/rootfs.tar.gz || fail_and_exit;
    fi

    if [ $UPDATE_EPC == 1 ]; then
        echo "Will deploy ePC updates."
        cp $EPC_UPDATE $OUT_DIRECTORY/EPC/update_5-7i3.tar && chmod 666 $OUT_DIRECTORY/EPC/update_5-7i3.tar || fail_and_exit;
        cp $EPC_2_UPDATE $OUT_DIRECTORY/EPC/update_10i3.tar && chmod 666 $OUT_DIRECTORY/EPC/update_10i3.tar || fail_and_exit;
    fi

    if [ $UPDATE_PLAYCONTROLLER == 1 ]; then
        echo "Will deploy playcontroller update."
        cp $PLAYCONTROLLER_UPDATE $OUT_DIRECTORY/playcontroller/main.bin && chmod 666 $OUT_DIRECTORY/playcontroller/main.bin || fail_and_exit;
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

    if [ $UPDATE_PLAYCONTROLLER == 1 ]; then
        cp $SOURCE_DIR/update_scripts/playcontroller_update.sh $OUT_DIRECTORY/playcontroller/ && \
            chmod 777 $OUT_DIRECTORY/playcontroller/playcontroller_update.sh && \
            cp $SOURCE_DIR/update_scripts/playcontroller_check.sh $OUT_DIRECTORY/playcontroller/ && \
            chmod 777 $OUT_DIRECTORY/playcontroller/playcontroller_check.sh || \
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

    for i in mxli sshpass text2soled rsync socat thttpd playcontroller; do
        if ! cp $(find $BINARY_DIR/build-tools/bbb/rootfs -type f -name "$i" | head -n 1) $OUT_DIRECTORY/utilities/; then
          echo "could not get $i from rootfs"
          return 1
        fi
    done

    for i in sshpass text2soled rsync socat thttpd mxli playcontroller; do
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

print_version_string()
{
    [ ! -z "$1" ] && echo " " $(grep -m 1 --binary-files=text "C15 Version" $1 | sed '$ s/\x00*$//') " (" $1 ")"
}

print_C15_version_strings() {
    echo "Getting version strings..."
    if [ $UPDATE_EPC == 1 ]; then
        FILE=$BINARY_DIR/build-tools/epc/tmp/usr/local/C15/playground/playground
        rm -f $FILE
        tar -C $BINARY_DIR/build-tools/epc/tmp --extract --file=$BINARY_DIR/build-tools/epc/update.tar ./update/NonLinuxOverlay.tar.gz
        tar -C $BINARY_DIR/build-tools/epc/tmp --extract --file=$BINARY_DIR/build-tools/epc/tmp/update/NonLinuxOverlay.tar.gz ./usr/local/C15/playground/playground
        FILE=$BINARY_DIR/build-tools/epc/tmp/usr/local/C15/playground/playground
        print_version_string $FILE
    fi
    if [ $UPDATE_PLAYCONTROLLER == 1 ]; then
        print_version_string $(find $BINARY_DIR/build-tools/playcontroller/ -type f -name "main.bin")
    fi
    if [ $UPDATE_BBB == 1 ]; then
        print_version_string $(find $BINARY_DIR/build-tools/bbb/rootfs/usr -type f -name "playcontroller")
        print_version_string $(find $BINARY_DIR/build-tools/bbb/rootfs/usr -type f -name "playcontroller-read")
        print_version_string $(find $BINARY_DIR/build-tools/bbb/rootfs/usr -type f -name "ehc")
        print_version_string $(find $BINARY_DIR/build-tools/bbb/rootfs/usr -type f -name "ehc-preset")
    fi
    echo "Getting version strings done."
}

main() {
    set -e
    check_preconditions || fail_and_exit
    create_update_file_structure || fail_and_exit
    deploy_updates || fail_and_exit
    deploy_scripts || fail_and_exit
    get_tools_from_rootfs || fail_and_exit
    create_update_tar || fail_and_exit
    print_C15_version_strings
}

main
