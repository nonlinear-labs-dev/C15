#!/bin/sh

UPDATE=$1
UNIT=$2
PASSWORD=$3
IP=192.168.8.2

echo "Trying to update $UNIT with update found in $UPDATE."

if ! nmcli device wifi connect $UNIT password $PASSWORD; then
    if ! sudo nmcli device wifi connect $UNIT password $PASSWORD; then
        echo "could not connect to unit $UNIT"
        exit 1
    fi
fi

if ! ping $IP -c 10 > /dev/null; then
    echo "could not ping unit $UNIT"
    exit 1
fi

ssh root@$IP "rm -rf /update && mkdir -p /update"
scp -C $UPDATE root@$IP:/update
ssh root@$IP "cd /update/ && tar xf nonlinear-c15-update.tar"
ssh root@$IP "chmod +x /update/run.sh"
ssh root@$IP '/update/run.sh > /dev/null &'
