/******************************************************************************/
/** @file           pe_env_engine.h
    @date           2018-03-19
    @version        0.1
    @author         Matthias Seeber
    @brief          envelope generator
                    (rendering mono and poly envelope signals according to
                    current states of involved parameters)
    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "pe_defines_config.h"
#include "pe_defines_envelopes.h"
#include "pe_utilities.h"

/* a header-like object for each envelope, holding crucial settings */
struct env_head
{
    /* index to first (voice) rendering item, object type and size (polyphony) */
    uint32_t m_index = 0;
    uint32_t m_type = 0;
    uint32_t m_size = 0;
    /* envelope segment definitions */
    uint32_t m_startIndex = 0;
    uint32_t m_stopIndex = 0;
    uint32_t m_state[sig_number_of_env_segments + 1];
    uint32_t m_next[sig_number_of_env_segments + 1];
    /* monophonic attack curve parameter implementation */
    float m_attackCurve = 0.f;
};

/* each envelope segment (or phase/stage) requires two (dynamic) parameters: slope (dx) and destination (dest) */
struct env_segment
{
    float m_dx = 0.f;
    float m_dest = 0.f;
};

/* body objects perform actual rendering and are instanciated for each envelope according to its polyphony (one body per voice) */
struct env_body
{
    /* currently, four segments are defined (Att, Dec1, Dec2, Rel), one additional IDLE segment */
    env_segment m_segment[sig_number_of_env_segments + 1];
    /* segment rendering is based on a state machine witch the following variables */
    uint32_t m_state = 0;
    uint32_t m_next = 0;
    uint32_t m_index = 0;
    /* actual rendering of a segment requires the following additional variables */
    float m_x = 0.f;
    float m_y = 0.f;
    float m_start = 0.f;
    float m_signal = 0.f;
};

/* main envelope rendering object, holding all required data */
struct env_engine
{
    /* voice management helper for initialization (voices are overwritten during initialization) */
    uint32_t m_routePolyphony[2] = {1, dsp_number_of_voices};
    /* local data structures */
    dual_env_id_list m_polyIds;
    env_head m_head[sig_number_of_envelopes];
    env_body m_body[sig_number_of_env_items];
    /* proper init */
    void init(const uint32_t _voices, const float _gateRelease);
    void init_adbdsr(const uint32_t _envId);
    void init_gate(const uint32_t _envId, const float _gateRelease);
    void init_decay(const uint32_t _envId);
    /* segment manipulation */
    void setSegmentDx(const uint32_t _voiceId, const uint32_t _envId, const uint32_t _segmentId, const float _value);
    void setSegmentDest(const uint32_t _voiceId, const uint32_t _envId, const uint32_t _segmentId, const float _value);
    /* main tick functions for rendering (envelopes will render at audio rate) */
    void tickMono();
    void tickPoly(const uint32_t _voiceId);
    /* individual rendering of an envelope item */
    void tickItem(const uint32_t _voiceId, const uint32_t _envId);
    /* segment traversal (going to subsequent segment after completing the current one) */
    void nextSegment(const uint32_t _voiceId, const uint32_t _envId);
    /* helper function in order to produce polynomial transitions */
    float squaredCurvature(const float _value, const float _curvature);
    /* main envelope triggers (relating to keyUp, keyDown) */
    void startEnvelope(const uint32_t _voiceId, const uint32_t _envId, const float _attackCurve, const float _retriggerHardness);
    void stopEnvelope(const uint32_t _voiceId, const uint32_t _envId);
};
