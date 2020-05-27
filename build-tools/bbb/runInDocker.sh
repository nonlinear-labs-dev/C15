#!/bin/sh

BINARY_DIR=$(realpath $1)
shift
SOURCE_DIR=$(realpath $1)
shift
DOCKER_ARGS="$1"
shift

SSH_DIR=$(realpath "$HOME/.ssh")

TMPSCRIPT=$(mktemp)
TMPSCRIPTBASE=$(basename $TMPSCRIPT)
TMPDIRNAME="$BINARY_DIR/tmp"
TMPSCRIPT="$TMPDIRNAME/$TMPSCRIPTBASE"
mkdir -p $TMPDIRNAME

# COPY local ssh files into the docker container to be able to push onto github
echo "
mkdir -p /docker-ssh
rsync -a /host-ssh/ /docker-ssh
chown -R root /docker-ssh
if ! grep \"/docker-ssh\" /etc/ssh/ssh_config; then
 echo \"IdentityFile /docker-ssh/id_rsa\" >> /etc/ssh/ssh_config
fi
" > $TMPSCRIPT

for var in "$@"
do
    echo $var >> $TMPSCRIPT
done

DOCKERNAME="nl-cross-build-environment"
docker run $DOCKER_ARGS --privileged --rm -v $SSH_DIR:/host-ssh -v $TMPDIRNAME:/script -v $BINARY_DIR:/workdir -v $SOURCE_DIR:/sources $DOCKERNAME bash /script/$TMPSCRIPTBASE
