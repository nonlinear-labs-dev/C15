/******************************************************************************/
/** @file       pe_defines_evnvelopes.h
    @date       2018-03-13
    @version    1.0
    @author     Matthias Seeber
    @brief      envelope object definition
                ..
    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "pe_defines_config.h"

/* envelope type definitions (three distinct envelope types) - define behavior for all segments */

#define sig_env_type_adbdsr 0
#define sig_env_type_gate 1
#define sig_env_type_decay 2

const uint32_t envelope_types[sig_number_of_env_types][sig_number_of_env_segments][4] = {
    //def:      state   next    dx      dest
    {   // adbdsr type
        {       3,      2,      0,      1       },                      // 1 polynomial attack phase to 100% (next: 2 - dec1)
        {       1,      3,      0,      0       },                      // 2 linear decay1 phase (next: 3 - dec2)
        {       2,      0,      0,      0       },                      // 3 exponential decay2 phase (infinite, no next)
        {       4,      0,      0,      0       }                       // 4 exponential release phase to 0% (next: 0 - idle)
    },
    {   // gate type
        {       1,      0,      1,      1       },                      // 1 linear attack phase to 100% in no time (next: 0)
        {       4,      0,      0,      0       },                      // 2 exponential release phase to 0% (next: 0 - idle)
        {       0,      0,      0,      0       },                      // (remaining unused)
        {       0,      0,      0,      0       }
    },
    {   // decay type
        {       1,      2,      1,      1       },                      // 1 linear attack phase to 100% in no time (next: 2)
        {       4,      0,      0,      0       },                      // 2 exponential release phase to 0% (next: 0 - idle)
        {       0,      0,      0,      0       },                      // (remaining unused)
        {       0,      0,      0,      0       }
    }
};

/* actual envelope definition (env type, poly type) */

const uint32_t envelope_definition[sig_number_of_envelopes][2] = {
    {0, 1},                                                             // poly adbdsr envelope ENV_A
    {0, 1},                                                             // poly adbdsr envelope ENV_B
    {0, 1},                                                             // poly adbdsr envelope ENV_C
    {1, 1},                                                             // poly gate envelope ENV_G
    {2, 0}                                                              // mono decay envelope ENV_F
};
