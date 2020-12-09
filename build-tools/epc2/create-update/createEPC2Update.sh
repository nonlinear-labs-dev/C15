#!/bin/bash

SELF_DIR=$(dirname $0)
DOCKERNAME="$1"
OUT_DIR=$(realpath $2)
SOURCES_DIR=$(realpath $3)
UPDATE_PACKAGES="$4"
FILESYSTEMS_DIR=$OUT_DIR/../target-os

SCRIPT=$(cat $SELF_DIR/createEPC2UpdateDockerCommands.sh)

if tty; then
    TTY_OPTION=" -ti "
fi

docker run $TTY_OPTION --privileged --env UPDATE_PACKAGES="$UPDATE_PACKAGES" -v $SOURCES_DIR:/source -v $FILESYSTEMS_DIR:/in -v $OUT_DIR:/out -v $OUT_DIR/build:/build ${DOCKERNAME} bash -c "$SCRIPT"
