/******************************************************************************/
/** @file       pe_defines_protocol.h
    @date       2018-03-13
    @version    1.0
    @author     Matthias Seeber
    @brief      TCD protocol definition (according to TCD specs 1.5)
                ..
    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>

// TCD_Command IDs:
#define t_V 1           // selectVoice(unsigned14 id)
#define t_VM 2          // selectMultipleVoices(unsigned14 id)
#define t_P 3           // selectParam(unsigned14 id)
#define t_PM 4          // selectMultipleParams(unsigned14 id)
#define t_T 5           // setTime(unsigned14 value)
#define t_TU 6          // setTimeUpper(unsigned14 upper)
#define t_TL 7          // setTimeLower(unsigned14 lower)
#define t_D 8           // setDestination(unsigned14 value)
#define t_DS 9          // setSignedDestination(signed14 value)
#define t_DU 10         // setDestinationUpper(signed14 upper)
#define t_DL 11         // setDestinationLower(unsigned14 lower)
#define t_PL 12         // preload(unsigned14 value)
#define t_KU 13         // keyUp(unsigned14 velocity)
#define t_KD 14         // keyDown(unsigned14 velocity)

// TCD_Command Candidates:
#define t_RST 15        // reset(mode):                                 not preloadable, triggering reset (either flush, envelope stop or complete dsp reset)
#define t_U 16          // selectUtility(unsigned14 id):                selection of utility parameters that require update triggers
#define t_UD 17         // setUtility(unsigned14 value):                actual utility parameter update, similar to setDestination
#define t_KV 18         // keyVoice(unsigned14 (voiceSteal, voiceId)    1.56 key update simplification command

// later: select time? (can be realized by utilities)
// later: select lists? (easing up list updates like preset recall (voice-parallel) or key event sequences (voice-serial))
// later: MB, MP (the morphing thing...) ???

/* TCD_Protocol Definition 1.56
  MIDI_CHANNEL:    -       -       -       -       -       -       -       -       -       -       -       -       -       -
    0      1       2       3       4       5       6       7       8       9       A       B       C       D       E       F
    -      -       -       -       -       -       -       -       -       -       -       -       -       -       -       -      */
const uint32_t tcd_protocol[128] = {

    // (valid) NOTE_OFF by Channel

    t_V,   t_P,    t_T,    0,      0,      t_D,    0,      t_KU,   t_U,    0,      0,      0,      0,      0,      0,      0,

    // (valid) NOTE_ON by Channel

    t_VM,  t_PM,   0,      0,      0,      t_DS,   0,      t_KD,   t_UD,   0,      0,      0,      0,      0,      0,      0,

    // (valid) POLY_AFTERTOUCH by Channel

    0,     0,      t_TU,   0,      0,      t_DU,   0,      t_RST,  0,      0,      0,      0,      0,      0,      0,      t_PL,

    // (valid) CONTROL_CHANGE by Channel

    0,     0,      t_TL,   0,      0,      t_DL,   0,      t_KV,   0,      0,      0,      0,      0,      0,      0,      0,

    // (invalid) PROGRAM_CHANGE by Channel

    0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,

    // (invalid) MONO_AFTERTOUCH by Channel

    0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,

    // (invalid) PITCH_BEND by Channel

    0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,

    // (invalid) SYS_EX and RESERVED

    0,     0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,      0,
};
