#!/bin/bash

set -e

DOCKERNAME="$1"
OUT_FILE=$(realpath $2)
OUT_DIR=$(dirname $2)
PACKAGES="$3"
IN_DIR="$4"
USER_ID=$(id -u)

if tty; then
    TTY_OPTION=" -ti "
fi

SCRIPT=$(cat $IN_DIR/createNonLinuxReleasePackagesDockerCommands.sh)

docker run $TTY_OPTION --privileged --env PACKAGES="$PACKAGES" -v $OUT_DIR:/out ${DOCKERNAME} bash -c "$SCRIPT"
mv $OUT_DIR/out.tar.gz $OUT_FILE


