#!/bin/sh

UPDATE=$1
UNIT=$2
PASSWORD=$3
IP=192.168.8.2

echo "Trying to update $UNIT with update found in $UPDATE."

if ! sudo nmcli device wifi connect $UNIT password $PASSWORD; then
    echo "could not connect to unit $UNIT"
    return 1
fi

if ! ping $IP -c 1 > /dev/null; then
    echo "could not ping unit $UNIT"
    return 1
fi

ssh root@$IP "ssh root@$IP rm -rf /update && mkdir -p /update"
scp -C $UPDATE root@$IP:/update
ssh root@$IP "cd /update/ && tar xvf nonlinear-c15-update.tar && chmod +x ./run.sh"
ssh root@$IP "cd /update/ && nohup /bin/sh ./run.sh &"