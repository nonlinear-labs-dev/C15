#!/bin/sh

BINARY_DIR=$1
SOURCE_DIR=$2
SCRIPT=$3
TMPSCRIPT=$(mktemp)
TMPDIRNAME=$(dirname $TMPSCRIPT)
TMPSCRIPTBASE=$(basename $TMPSCRIPT)

USER_ID=$(id -u $USER)
DOCKERNAME="nl-epc-build-environment-$USER"

cp $SCRIPT $TMPSCRIPT

docker run -e TARGETUSER=$USER --privileged --rm -v $TMPDIRNAME:/script -v $BINARY_DIR:/workdir -v $SOURCE_DIR:/sources \
$DOCKERNAME bash /script/$TMPSCRIPTBASE
