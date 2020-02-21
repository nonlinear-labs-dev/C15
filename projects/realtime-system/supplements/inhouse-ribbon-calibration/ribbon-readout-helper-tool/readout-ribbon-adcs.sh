#!/bin/sh
echo
echo "First, make sure we are connected to a C15 via WiFi!"
echo "Establish a ground connection between you body and the C15, to minimize noise"
echo
read -p "Press [Enter] to start calibration readout or [CTRL+c] to abort:" dummy
echo
ssh root@192.168.8.2 "systemctl stop bbbb"
ssh root@192.168.8.2 "systemctl stop playground"
ssh root@192.168.8.2 "kill -n 15 \`pidof bbbb.cal\`"
scp bbbb.cal root@192.168.8.2:/tmp
scp raw-sensors-on.bbmsg root@192.168.8.2:/tmp
scp raw-sensors-on_LPC120.bbmsg root@192.168.8.2:/tmp
ssh root@192.168.8.2 "cat /tmp/raw-sensors-on.bbmsg > /dev/lpc_bb_driver"
ssh root@192.168.8.2 "cat /tmp/raw-sensors-on_LPC120.bbmsg > /dev/lpc_bb_driver"
echo
echo "To quit calibration press [CTRL+c], then restart"
echo "the C15 to resume to normal operation."
echo
echo "Touch the ribbons and see if you get stable readout values"
echo "(first column is for the upper ribbon, second for lower)."
ssh root@192.168.8.2 "/tmp/bbbb.cal -l" | ./ribbon-filter
