#!/bin/sh
#set -x

function export_gpio_for_write {
	GPIO=$1
	echo $GPIO > /sys/class/gpio/export
	sleep 0.1
	echo out > /sys/class/gpio/gpio$GPIO/direction
	sleep 0.1
}
function export_gpio_for_read {
	GPIO=$1
	echo $GPIO > /sys/class/gpio/export
	sleep 0.1
	echo in > /sys/class/gpio/gpio$GPIO/direction
	sleep 0.1
}
function unexport_gpio {
	GPIO=$1
	echo $GPIO > /sys/class/gpio/unexport 2> /dev/null
	sleep 0.1
}
function write_to_gpio {
	GPIO=$1
	VALUE=$2
	echo $VALUE > /sys/class/gpio/gpio$GPIO/value
	sleep 0.1
}

# stop any leftover pin togglers
while kill -n 15 `ps - ef | grep -v grep | grep toggler | awk '{print $1}'` 2> /dev/null
do
  :
done


TOGGLE=61
nOE2=75
POWER=70
RESET=74
SCK=72
MOSI=73
MISO=71

# free our pins in case they have been aquired by an aborted previous run
unexport_gpio $TOGGLE
unexport_gpio $nOE2
unexport_gpio $POWER

# free pins used by AVRdude in case they have been aquired by an aborted previous run of AVRdude
unexport_gpio $RESET
unexport_gpio $SCK
unexport_gpio $MOSI
unexport_gpio $MISO

# export our pins for writing
export_gpio_for_write $nOE2
export_gpio_for_write $POWER


# prepare progamming, enable interface
#
# set POWER high
write_to_gpio $POWER 1
#set /OE2 low
write_to_gpio $nOE2 0


# call pin toggler and let run in background
/nonlinear/utilities/toggler &

# wait until toggling is effective
sleep 1


# program
/bin/sh /nonlinear/utilities/program-avr.sh


# exit programming, disable interface
#set /OE2 high
write_to_gpio $POWER 1
# set POWER high
write_to_gpio $nOE2 1

# stop pin toggler
while kill -n 15 `ps - ef | grep -v grep | grep toggler | awk '{print $1}'` 2> /dev/null
do
  :
done
unexport_gpio $TOGGLE

# wait until toggling is in-effective
sleep 1

unexport_gpio $nOE2
unexport_gpio $POWER
