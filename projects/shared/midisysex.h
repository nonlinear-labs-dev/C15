#pragma once

#include <stddef.h>
#include <stdint.h>
#include "lpc-defs.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*

 USB block msgs : ... -->|<---------- USB #1 --------->|<---------- USB #2 --------->
 SysEx messages : ... SysEx #1 -------->|<-- SysEx #2 -->|<-- SysEx #3 -->|<-- ...
 BB messages    : ... BBmsg #1 -------->|<-- BBmsg #2 -->|<-- BBmsg #3 -->|<-- ...
 
 USB message bulk blocks may be completely unsynced with the contained SysEx and other MIDI msgs.
 For simplicity, SysEx msgs are mapped 1:1 to BB msgs, that 1 SysEx <==> 1 BB msg.
 Size of BB msgs is restricted to 1020bytes (510 words) in the same way as the traditional BB messages over SPI,
 this means after encoding the size increases to ~1.2x --> 610bytes. Packed in USB midi packets makes another ~1.4x,
 summing up to ~1kB USB bulk total, which is 2 or 3 bulk packets (each can be 512bytes at most).

 The smallest BB msg is 3 words (type, length, data) --> 6 bytes to be packed in a SysEx --> 7bytes raw SysEx.
 Plus leading and tailing marker we have 9 bytes to transmit, which means 3 4-byte USB frames:
   04 | F0 hb b1
   04 | b1 b3 b4
   07 | b5 b6 F7
 So, transfer efficiency for smallest BB msgs is 6/12 = 0.5.

 The next smallest BB msg is 4 words (type, length, data1, data2) --> 8 bytes (p) to be packed in a SysEx:
 p1 ... p8 --> hb b1 b2 b3 b4 b5 b6 b7 hb b8   ("hb" means the high bits for the following 1..7 bytes)
 
 Plus leading and tailing marker we have 11 7bit-bytes (b) to transmit, which means 4 4-byte USB frames:
   04 | F0 hb b1
   04 | b2 b3 b4
   04 | b5 b6 b7
   07 | hb b8 F7
 So, transfer efficiency for smallest BB msgs is 8/16 = 0.5, again.
 
 Since the two smallest BB msgs by chance take up all bytes in USB frames, efficiency is OK
 A 5 word BB messages will be encoded like this:
   04 | F0  hb  b1
   04 | b2  b3  b4
   04 | b5  b6  b7
   04 | hb  b8  b9
   06 | b10 F7  00
 Here, efficiency is 10/20 = 0.5 again
 
 6 Words:
   04 | F0  hb  b1
   04 | b2  b3  b4
   04 | b5  b6  b7
   04 | hb  b8  b9
   04 | b10 b11 b12
   05 | F7  00  00
 Efficiency still at 12/24 = 0.5
 
 7 Words :
   04 | F0  hb  b1
   04 | b2  b3  b4
   04 | b5  b6  b7
   04 | hb  b8  b9
   04 | b10 b11 b12
   07 | b13 b14 F7
 Finally, efficiency increases to 14/24 = 0.6

 8 Words :
   04 | F0  hb  b1
   04 | b2  b3  b4
   04 | b5  b6  b7
   04 | hb  b8  b9
   04 | b10 b11 b12
   04 | b13 b14 hb
   07 | b15 b16 F7
 Efficiency: 16/28 = 0.57
 
 Therefore, it seems there would be no significant benefit when more than one BB msg were be contained in a SysEx
 or even when SysEx and BB msgs streams were completely independant.
 
*/


// Callback for putting each encoded byte into the appropriate buffer scheme
// On LPC, this will assemble USB frames and enqueue the resulting buffer(s), lastFlag > 0 is required to
// setup the last USB frame properly. lastFlag < 0 just resets the state machine.
//
// On ePC, this will collect the bytes as is in a local buffer and issue a call to AlsaMidiOutput::send()
// when finished (lastFlag > 0). lastFlag < 0 resets state machine (if any).
typedef void (*StoreSysExOutputCallback)(uint8_t const data, int8_t const lastFlag);

void MIDI_SetStoreSysExOutputCallback(StoreSysExOutputCallback const cb);
void MIDI_EncodeSysEx(uint8_t * rawData, size_t bytes);  // calls the above callback for each encoded output byte


// Callback doing something with a completed SysEx message
//
// On LPC and EPC, these will invoke the BB message handler pretty much directly
typedef void (*DecodeSysExFinishedCallback)(uint8_t const *data, size_t bytes);

void   MIDI_SetDecodeSysExFinishedCallback(DecodeSysExFinishedCallback const cb);
void   MIDI_ResetDecodeSysEx(void);
size_t MIDI_DecodeSysEx(uint8_t const rawByte);  // state machine scanning incoming data for SysEx delimiters, call above callback when finished


#ifdef __cplusplus
}
#endif
