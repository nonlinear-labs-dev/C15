#!/bin/sh

# verions : 2.0
# changes : append to /update/mxli.log to allow for multiple flashing reports in one file
#
# ---------- in-service programming the RT firmware into the playcontroller chip -----------
# errors will be logged and reported
#

PLAYCONTROLLER_FIRMWARE=$1
if [ -z "$1" ] ; then
    echo "no argument supplied, expected path to playcontroller firmware blob!"
	echo "E31 RT update: no argument" >> /update/errors.log
	exit 31
fi

if [ ! -f $PLAYCONTROLLER_FIRMWARE ] ; then
    echo "no file in $PLAYCONTROLLER_FIRMWARE"
	echo "E32 RT update: no file" >> /update/errors.log
	exit 32
fi
 
PLAYCONTROLLER_BANK=$2
if [ -z "$2" ] ; then
	echo "no bank, expected either A or B!"
	echo "E34 RT update: no bank selected" >> /update/errors.log
	exit 34
fi

export_gpio() {
	GPIO=$1
	echo $GPIO > /sys/class/gpio/export
	sleep 0.1
	echo out > /sys/class/gpio/gpio$GPIO/direction
	sleep 0.1
}

unexport_gpio() {
	GPIO=$1
	echo $GPIO > /sys/class/gpio/unexport
	sleep 0.1
}

write_to_gpio() {
	GPIO=$1
	VALUE=$2
	echo $VALUE > /sys/class/gpio/gpio$GPIO/value
	sleep 0.1
}

write_blob() {
    # in-service programming of the playcontroller, errors are appended to file "/update/mxli.log"
        /update/utilities/mxli -v -d /dev/ttyUSB0 -N LPC4337 -E -b115200 -T500 -c12000000 -F8kix8,64kix7 -B1024,4096 -A0x1A000000,0x1B000000 -M32ki@0x10000000 -I0xA001CA30,0xXXXXXX00 -R0x200@0x1008000,-288@0x10000000 -S8@7 -y$PLAYCONTROLLER_BANK $PLAYCONTROLLER_FIRMWARE 2>> /update/mxli.log
	return $?
}

# we handle 2 GPIO pins for the ISP signal so that it works
# with new uniboard revisions where ISP is wired differently.
# Root cause of rewiring is that no all BBB hardware implementations
# correctly handle the GPIOs with our specific device tree and
# a new pin has been choosen that seemingly works with the affected
# BBB hardware implementation
#
# ISP GPIO pin for uniboards < 7.2 (RevD)
ISP1=48
# ISP GPIO pin for uniboards >= 7.2 (RevD)
ISP2=51

# Unfortunately the previous version of the script blocks 
# the PC programming if GPIOs are not unexported at the end.
export_gpio $ISP1 2> /dev/null
export_gpio $ISP2 2> /dev/null
unexport_gpio $ISP1 2> /dev/null
unexport_gpio $ISP2 2> /dev/null

RESET=50
PROGMODE=115
export_gpio $ISP1 2> /dev/null
export_gpio $ISP2 2> /dev/null
export_gpio $RESET
export_gpio $PROGMODE
write_to_gpio $ISP1 1
write_to_gpio $ISP2 1
write_to_gpio $RESET 1
write_to_gpio $PROGMODE 1
write_to_gpio $ISP1 0
write_to_gpio $ISP2 0
write_to_gpio $RESET 0
write_to_gpio $RESET 1
write_to_gpio $ISP1 1
write_to_gpio $ISP2 1

write_blob
return_code=$?

write_to_gpio $PROGMODE 0
write_to_gpio $RESET 0
write_to_gpio $RESET 1
unexport_gpio $PROGMODE
unexport_gpio $RESET

unexport_gpio $ISP1
unexport_gpio $ISP2

if [ $return_code -eq 0 ] ; then
	exit 0
else
    echo "E33 RT update: cannot connect to playcontroller, details:" >> /update/errors.log
	cat /update/mxli.log >> /update/errors.log
	exit 33
fi
