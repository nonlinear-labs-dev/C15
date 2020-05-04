#!/bin/sh

BINARY_DIR=$(realpath $1)
SOURCE_DIR=$(realpath $2)
SCRIPT=$(realpath $3)

TMPSCRIPT=$(mktemp)
TMPSCRIPTBASE=$(basename $TMPSCRIPT)
TMPDIRNAME="$BINARY_DIR/tmp"
TMPSCRIPT="$TMPDIRNAME/$TMPSCRIPTBASE"
mkdir -p $TMPDIRNAME

USER_ID=$(id -u $USER)
DOCKERNAME="nl-epc-build-environment-$USER"

cp $SCRIPT $TMPSCRIPT

docker run -e TARGETUSER=$USER --privileged --rm -v $TMPDIRNAME:/script -v $BINARY_DIR:/workdir -v $SOURCE_DIR:/sources \
$DOCKERNAME bash /script/$TMPSCRIPTBASE
