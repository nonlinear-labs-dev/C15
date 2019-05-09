#!/bin/sh

cd /tmp

rm -rf ./error-log
rm -rf ./error-log.tar
rm -rf ./error-log.tar.gz

mkdir -p ./error-log

wget -q "http://192.168.8.2:80/get-update?updateID=0" -O ./error-log/update.xml
journalctl -u playground -b0 > ./error-log/journal.log

tar -cf error-log.tar error-log > /dev/null 2>&1 
gzip -c error-log.tar

 
