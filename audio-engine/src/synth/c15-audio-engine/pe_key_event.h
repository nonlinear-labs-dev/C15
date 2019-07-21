/******************************************************************************/
/** @file       pe_key_event.h
    @date       2018-03-16
    @version    1.0
    @author     Matthias Seeber
    @brief      key event data structures, preload-capable, mono and poly
                (hold time and level factors for envelope segments)
    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "pe_defines_config.h"

/* basic key event data structure */
struct key_event
{
    /* local variables */
    uint32_t m_preload = 0;
    uint32_t m_type = 0;
    float m_velocity = 0;
};

/* basic envelope event data structure */
struct env_event
{
    /* local variables */
    float m_timeFactor[dsp_number_of_voices][sig_number_of_env_segments];
    float m_levelFactor[dsp_number_of_voices];
    /* proper init (filling given arrays with zeros to avoid trouble) */
    void init();
};

/* main key event data structure (handling in parent parameter engine) */
struct poly_key_event
{
    /* local data structures */
#if test_flanger_env_legato
    uint32_t m_active = 0;                      // tracking active keys for legato flanger env behavior
#endif
    key_event m_mono;
    key_event m_poly[dsp_number_of_voices];
    env_event m_env[sig_number_of_env_events];
};
