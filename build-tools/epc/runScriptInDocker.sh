#!/bin/sh

BINARY_DIR=$(realpath $1)
SOURCE_DIR=$(realpath $2)
SCRIPT=$(realpath $3)

TMPSCRIPT=$(mktemp)
TMPSCRIPTBASE=$(basename $TMPSCRIPT)
TMPDIRNAME="$BINARY_DIR/tmp"
TMPSCRIPT="$TMPDIRNAME/$TMPSCRIPTBASE"
mkdir -p $TMPDIRNAME

DOCKERNAME="nl-epc-build-environment"

cp $SCRIPT $TMPSCRIPT

GROUP_ID=$(id -g $USER)
USER_ID=$(id -u $USER)

docker run -ti -e GROUP_ID=$GROUP_ID -e USER_ID=$USER_ID --privileged --rm -v $TMPDIRNAME:/script -v $BINARY_DIR:/bindir -v $SOURCE_DIR:/sources \
$DOCKERNAME bash /script/$TMPSCRIPTBASE
