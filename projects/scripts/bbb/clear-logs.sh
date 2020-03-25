#!/bin/sh

#
# last changed: 2018-11-16 KSTR
# version : 1.0
#
# ---------- Clear Log Files -----------
#
#

journalctl --flush
journalctl --rotate
journalctl --vacuum-time=1seconds