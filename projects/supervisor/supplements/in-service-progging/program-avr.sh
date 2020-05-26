#!/bin/sh

avrdude -p m16 -c linuxgpio -P dummy -q -U flash:w:Supervisor-V5.2.elf
