#!/bin/bash

set -e

DOCKERNAME="$1"
OUT_FILE=$(realpath $2)
OUT_DIR=$(dirname $2)
PACKAGES="$3"
USER_ID=$(id -u)

if tty; then
    TTY_OPTION=" -ti "
fi

docker run $TTY_OPTION --privileged -v $OUT_DIR/pac-cache:/var/cache/pacman -v $OUT_DIR/pac-conf:/var/lib/pacman ${DOCKERNAME} bash -c "\
rm -rf /var/cache/pacman; 
rm -rf /var/lib/pacman; 
pacman --noconfirm -Syy;
pacman --noconfirm -Sw ${PACKAGES};
chown -R $USER_ID:$USER_ID /var/cache/pacman;
chown -R $USER_ID:$USER_ID /var/lib/pacman;
"

tar -C $OUT_DIR -czvf $OUT_FILE ./pac-conf ./pac-cache
