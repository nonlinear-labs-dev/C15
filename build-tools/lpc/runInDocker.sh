#!/bin/sh

BINARY_DIR=$1
shift
SOURCE_DIR=$1
shift
USER_ID=$1
shift
GROUP_ID=$1
shift

SCRIPT="echo \"Starting script:\""

for var in "$@"
do
    SCRIPT="$SCRIPT && $var"
done

SCRIPT="$SCRIPT && echo \"Script done.\""
DOCKERNAME="nl-lpc-cross-build-environment-$USER"
docker run --privileged -u $USER_ID:$GROUP_ID --rm -v $BINARY_DIR:/workdir -v $SOURCE_DIR:/sources $DOCKERNAME bash -c "$SCRIPT"
