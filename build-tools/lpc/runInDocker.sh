#!/bin/sh

BINARY_DIR=$(realpath $1)
shift
SOURCE_DIR=$(realpath $1)
shift

SCRIPT="echo \"Starting script:\""

for var in "$@"
do
    SCRIPT="$SCRIPT && $var"
done

SCRIPT="$SCRIPT && echo \"Script done.\""
DOCKERNAME="nl-lpc-cross-build-environment"
docker run --privileged --rm -v $BINARY_DIR:/workdir -v $SOURCE_DIR:/sources $DOCKERNAME bash -c "$SCRIPT"
