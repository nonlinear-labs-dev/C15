#!/bin/sh
cat key-down-C3-2.5ms.lpcmsg > /dev/lpc_bb_driver; sleep 0.5; cat key-up-C3-250ms.lpcmsg > /dev/lpc_bb_driver
