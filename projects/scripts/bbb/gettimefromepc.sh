#!/bin/sh

getEPCtime(){
        sshpass -p 'sscl' ssh sscl@192.168.10.10 'date "+%F %T"'
}

date "+%F %T" -s "$(getEPCtime)"
