#!/bin/bash

SRC_DIR=$(realpath $1)
OUT_DIR=$(realpath $2)
IN_DIR=$(realpath $3)

if tty; then
    TTY_OPTION=" -ti "
fi

SCRIPT=$(cat $SRC_DIR/createInstallMediumDockerCommands.sh)
docker run $TTY_OPTION --privileged -v $OUT_DIR:/out -v $IN_DIR:/in -v $SRC_DIR:/src archlinux:20200908 bash -c "$SCRIPT"