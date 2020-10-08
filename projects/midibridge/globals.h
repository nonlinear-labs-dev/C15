// system-wide globals / defines / compile switches
#pragma once

// LPC compile time switches, feature options :
#define LPC_KEYBED_DIAG (0)  // enable key event counter diagnostics

// LPC compile time switches, debug specials (breaking normal function) :
#define LPC_DGB_ADC_STRESS_TEST (0)  // use emulated ADC values for stress testing

// LPC compile time switches, select USB MIDI port for "EMPHASE" device driver
// Select port #0 (0) or port #1 (1) for USB MIDI connection. 0 is the default for current uniboards.
// Port #1 will result in an only FullSpeed device, port #0 is HighSpeed
#define USB_PORT_FOR_MIDI (0)
