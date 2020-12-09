#!/bin/bash

DOCKERNAME="$1"
OUT_DIR=$(realpath $2)
IN_DIR=$(realpath $3)
PACKAGES_DIR=$OUT_DIR/../host-os
INSTALL_PACKAGES="$4"

SCRIPT=$(cat $IN_DIR/createNonLinuxOSDockerCommands.sh)

if tty; then
    TTY_OPTION=" -ti "
fi

docker run $TTY_OPTION --privileged --env INSTALL_PACKAGES="$INSTALL_PACKAGES" -v $PACKAGES_DIR/pac-cache:/var/cache/pacman -v $PACKAGES_DIR/pac-conf:/var/lib/pacman -v $OUT_DIR:/out -v $IN_DIR:/in ${DOCKERNAME} bash -c "$SCRIPT"
    