#!/bin/bash

SRC_DIR=$(realpath $1)
OUT_DIR=$(realpath $2)
IN_DIR=$(realpath $3)
ARCH_VERSION="$4"

if tty; then
    TTY_OPTION=" -ti "
fi

SCRIPT=$(cat $SRC_DIR/createInstallMediumDockerCommands.sh)
docker run $TTY_OPTION --privileged -v $OUT_DIR:/out -v $IN_DIR:/in -v $SRC_DIR:/src nl-epc2-${ARCH_VERSION}-host-os bash -c "$SCRIPT"