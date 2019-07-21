/*****************************************************************************/
/** @file       pe_env_engine2.h
 *  @date       2018-06-27
 *  @version    0.1
 *  @author     Matthias Seeber
 *  @brief      Envelope Generator
 *
 * rendering mono and poly envelope signals according to current values of
 * involved parameters, ADBDSR-type envelopes now feature retrigger and split
 * functionality
 *
 *  @todo       implenent parameter updates (paramengine)
******************************************************************************/

#pragma once

#include <stdint.h>
#include "nltoolbox.h"
#include "pe_defines_config.h"

/********************************************** Initial Segment Data **********************************************/

/* constant integer array defining each segment data for each envelope type */

const uint32_t envelope2_definition[sig_number_of_env_types][sig_number_of_env_segments][4] = {

    {   // ADBDSR Types:
        //  State   Next    Dx      Dest                        Description
        {   4,      2,      0,      0       },              //  polynomial attack phase
        {   1,      3,      0,      0       },              //  linear decay1 phase
        {   2,      0,      0,      0       },              //  quasi-infinite exponential decay2 phase
        {   3,      0,      0,      0       }               //  quasi-finite exponential release phase
    },

    {   // GATE Types:
        //  State   Next    Dx      Dest                        Description
        {   1,      0,      1,      1       },              //  linear attack phase
        {   2,      0,      0,      0       },              //  quasi-finite exponential release phase
        {   0,      0,      0,      0       },              //  n/a
        {   0,      0,      0,      0       }               //  n/a
    },

    {   // DECAY Types:
        //  State   Next    Dx      Dest                        Description
        {   1,      2,      1,      1       },              //  linear attack phase
        {   2,      0,      0,      0       },              //  quasi-finite exponential release phase
        {   0,      0,      0,      0       },              //  n/a
        {   0,      0,      0,      0       }               //  n/a
    }

};

/********************************************** Envelope Segment Structures **********************************************/

/* Single Segment Object (handling only magnitude signal) */

struct env_segment_single
{
    /* local segment pseudo-pointers */

    uint32_t m_state = 0;                                   // segment rendering state and curve type
    uint32_t m_next = 0;                                    // subsequent segment index (following segment)

    /* local rendering variables */

    float m_dx[dsp_number_of_voices] = {};                  // transition times as incremental values [0 ... 1] (infinite ... zero)
    float m_dest_magnitude[dsp_number_of_voices] = {};      // transition destinations for the magnitude signal
    float m_curve = 0.f;                                    // curvature (of the attack segment)
};

/* Single Monophonic Segment Object (handling only magnitude signal) */

struct env_segment_single_mono
{
    /* local segment pseudo-pointers */

    uint32_t m_state = 0;                                   // segment rendering state and curve type
    uint32_t m_next = 0;                                    // subsequent segment index (following segment)

    /* local rendering variables */

    float m_dx[1] = {};                                     // transition times as incremental values [0 ... 1] (infinite ... zero)
    float m_dest_magnitude[1] = {};                         // transition destinations for the magnitude signal
};

/* Split Segment Object (handling magnitude and timbre signals) */

struct env_segment_split : env_segment_single
{
    /* additional local rendering variables */

    float m_dest_timbre[dsp_number_of_voices] = {};         // additional transition destinations for the timbre signal
};

/********************************************** Envelope Rendering Body Structures **********************************************/

/* Single Body Object (rendering only magnitude signal) */

struct env_body_single
{
    /* local state variables */

    uint32_t m_state = 0;                                   // current rendering state and curve type
    uint32_t m_next = 0;                                    // subsequent segment index (following segment)
    uint32_t m_index = 0;                                   // current segment index

    /* local rendering variables */

    float m_x = 0.f;                                        // transition curve progress (linear and polynomial curves)
    float m_y = 0.f;                                        // transition curve progress (exponential curves)
    float m_start_magnitude = 0.f;                          // transition signal startpoint (magnitude)
    float m_signal_magnitude = 0.f;                         // rendering signal (magnitude)
};

/* Split Body Object (rendering magnitude and timbre signals) */

struct env_body_split : env_body_single
{
    /* additional local rendering variables */

    float m_start_timbre = 0.f;                             // transition signal startpoint (timbre)
    float m_signal_timbre = 0.f;                            // rendering signal (timbre)
};

/********************************************** Envelope Object Structures **********************************************/

/* ADBDSR Split Envelope Object (currently intended for Env A, B) */

struct env_object_adbdsr_split
{
    /* predefined segment indices */

    const uint32_t m_startIndex = 1;                        // segment index of the first segment on key down
    const uint32_t m_stopIndex = 4;                         // segment index of the final segment on key up

    /* split handling variables */

    float m_peakLevels[dsp_number_of_voices] = {};          // holding current peak levels
    float m_splitValues[2] = {};                            // holding crossfade values [0 ... 1] for magnitude and timbre

    /* local data structures: rendering body, segment array */

    env_body_split m_body[dsp_number_of_voices];
    env_segment_split m_segment[sig_number_of_env_segments + 1];

    /* object functionality */

    void start(const uint32_t _voiceId);
    void stop(const uint32_t _voiceId);
    void tick(const uint32_t _voiceId);
    void nextSegment(const uint32_t _voiceId);
    void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value);
    void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const bool _splitMode, const float _value);
    void setSplitValue(const float _value);
    void setAttackCurve(const float _value);
    void setPeakLevel(const uint32_t _voiceId, const float _value);
};

/* ADBDSR Retrigger Envelope Object (currently intended for Env C) */

struct env_object_adbdsr_retrig
{
    /* predefined segment indices */

    const uint32_t m_startIndex = 1;                        // segment index of the first segment on key down
    const uint32_t m_stopIndex = 4;                         // segment index of the final segment on key up

    /* retrigger handling variable */

    float m_retriggerHardness = 0.f;                        // crossfade value [0 ... 1] for retrigger behavior

    /* local data structures: rendering body, segment array */

    env_body_single m_body[dsp_number_of_voices];
    env_segment_single m_segment[sig_number_of_env_segments + 1];

    /* object functionality */

    void start(const uint32_t _voiceId);
    void stop(const uint32_t _voiceId);
    void tick(const uint32_t _voiceId);
    void nextSegment(const uint32_t _voiceId);
    void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value);
    void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _value);
    void setAttackCurve(const float _value);
    void setRetriggerHardness(const float _value);
};

/* Gate Envelope Object (currently intended for Gate) */

struct env_object_gate
{
    /* predefined segment indices */

    const uint32_t m_startIndex = 1;                        // segment index of the first segment on key down
    const uint32_t m_stopIndex = 2;                         // segment index of the final segment on key up

    /* local data structures: rendering body, segment array */

    env_body_single m_body[dsp_number_of_voices];
    env_segment_single m_segment[sig_number_of_env_segments + 1];

    /* object functionality */

    void start(const uint32_t _voiceId);
    void stop(const uint32_t _voiceId);
    void tick(const uint32_t _voiceId);
    void nextSegment(const uint32_t _voiceId);
    void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value);
    void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _value);
};

/* Decay Envelope Object (forcably monophonic, currently intended for Flanger Decay) */

struct env_object_decay
{
    /* predefined segment indices */

    const uint32_t m_startIndex = 1;                        // segment index of the first segment on key down

    /* local data structures: rendering body, segment array */

    env_body_single m_body[1];
    env_segment_single_mono m_segment[sig_number_of_env_segments + 1];

    /* object functionality */

    void start(const uint32_t _voiceId);
    void tick(const uint32_t _voiceId);
    void nextSegment(const uint32_t _voiceId);
    void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value);
    void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _value);
};

/********************************************** Envelope Engine Structure **********************************************/

/* Main Engine Object, rendering all Envelope Signals */

struct env_engine2
{
    /* local data structures: each individual envelope */

    env_object_adbdsr_split m_env_a;                                // Envelope A
    env_object_adbdsr_split m_env_b;                                // Envelope B
    env_object_adbdsr_retrig m_env_c;                               // Envelope C
    env_object_gate m_env_g;                                        // Envelope G (Gate)
    env_object_decay m_env_f;                                       // Envelope F (Flanger)

    /* object functionality (init) */

    void init(const uint32_t _voices, const float _gateRelease);    // proper initialization

    /* object functionality (rendering) */

    void tickMono();                                                // rendering function for monophonic Envelopes
    void tickPoly(const uint32_t _voiceId);                         // rendering function for polyphonic Envelopes
};
