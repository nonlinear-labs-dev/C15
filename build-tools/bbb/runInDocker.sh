#!/bin/sh

BINARY_DIR=$1
shift
SOURCE_DIR=$1
shift
USER_ID=$1
shift
DOCKER_GROUP_ID=$1
shift

TMPSCRIPT=$(mktemp)
TMPDIRNAME=$(dirname $TMPSCRIPT)
TMPSCRIPTBASE=$(basename $TMPSCRIPT)
echo "" > $TMPSCRIPT

for var in "$@"
do
    echo $var >> $TMPSCRIPT
done

DOCKERNAME="nl-cross-build-environment-$USER"
docker run --privileged -u $USER_ID:$DOCKER_GROUP_ID --rm -v $TMPDIRNAME:/script -v $BINARY_DIR:/workdir -v $SOURCE_DIR:/sources $DOCKERNAME bash /script/$TMPSCRIPTBASE
