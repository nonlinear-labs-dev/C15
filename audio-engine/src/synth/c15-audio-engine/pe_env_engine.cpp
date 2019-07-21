#include <math.h>
#include "pe_env_engine.h"

/* proper init */
void env_engine::init(const uint32_t _voices, const float _gateRelease)
{
    /* crucial indices for init */
    uint32_t i = 0;     // (rendering) item index
    uint32_t e;         // envelope index
    /* prepare polyphony routing array */
    m_routePolyphony[1] = _voices;
    /* traverse definition array (see pe_defines_envelopes.h) */
    for(e = 0; e < sig_number_of_envelopes; e++)
    {
        /* create reference */
        env_head* obj = &m_head[e];
        /* declare envelope head (common settings) */
        obj->m_index = i;
        obj->m_type = envelope_definition[e][0];
        obj->m_size = m_routePolyphony[envelope_definition[e][1]];
        /* add envelope id according to envelope polyphony */
        m_polyIds.add(envelope_definition[e][1], e);
        /* further init by type (individual settings) */
        switch(obj->m_type)
        {
        case 0:
            /* ADBDSR type (Envelopes A, B, C) */
            init_adbdsr(e);
            break;
        case 1:
            /* GATE type (Envelope G) */
            init_gate(e, _gateRelease);
            break;
        case 2:
            /* DECAY type (Flanger Envelope) */
            init_decay(e);
            break;
        }
        /* update (rendering) item index (by envelope polyphony) */
        i += obj->m_size;
    }
}

/* ADBDSR type initialization */
void env_engine::init_adbdsr(const uint32_t _envId)
{
    /* provide (rendering) item index and object reference */
    uint32_t index;
    env_head* obj = &m_head[_envId];
    /* initialize IDLE segment */
    obj->m_state[0] = 0;
    obj->m_next[0] = 0;
    /* initialize adbdsr start and stop indices */
    obj->m_startIndex = 1;
    obj->m_stopIndex = 4;
    /* outer loop - envelope (non-idle) segment initialization */
    for(uint32_t s = 0; s < sig_number_of_env_segments; s++)
    {
        /* segment definition */
        obj->m_state[s + 1] = envelope_types[sig_env_type_adbdsr][s][0];
        obj->m_next[s + 1] = envelope_types[sig_env_type_adbdsr][s][1];
        /* get (rendering) item index */
        index = obj->m_index;
        /* inner loop - initialize (rendering) items */
        for(uint32_t i = 0; i < obj->m_size; i++)
        {
            /* provide item reference */
            env_body* item = &m_body[index];
            /* item segment definition */
            item->m_segment[s + 1].m_dx = static_cast<float>(envelope_types[sig_env_type_adbdsr][s][2]);
            item->m_segment[s + 1].m_dest = static_cast<float>(envelope_types[sig_env_type_adbdsr][s][3]);
            /* update item index */
            index++;
        }
    }
}

/* GATE type initialization */
void env_engine::init_gate(const uint32_t _envId, const float _gateRelease)
{
    /* provide (rendering) item index and object reference */
    uint32_t index;
    env_head* obj = &m_head[_envId];
    /* initialize IDLE segment */
    obj->m_state[0] = 0;
    obj->m_next[0] = 0;
    /* initialize gate start and stop indices */
    obj->m_startIndex = 1;
    obj->m_stopIndex = 2;
    /* outer loop - envelope (non-idle) segment initialization */
    for(uint32_t s = 0; s < sig_number_of_env_segments; s++)
    {
        /* segment definition */
        obj->m_state[s + 1] = envelope_types[sig_env_type_gate][s][0];
        obj->m_next[s + 1] = envelope_types[sig_env_type_gate][s][1];
        /* get (rendering) item index */
        index = obj->m_index;
        /* inner loop - initialize (rendering) items */
        for(uint32_t i = 0; i < obj->m_size; i++)
        {
            /* provide item reference */
            env_body* item = &m_body[index];
            /* item segment definition - distinct release case */
            if(s < 1) {
                /* attack segment */
                item->m_segment[s + 1].m_dx = static_cast<float>(envelope_types[sig_env_type_gate][s][2]);
            }
            else {
                /* release segment */
                item->m_segment[s + 1].m_dx = _gateRelease;
            }
            item->m_segment[s + 1].m_dest = static_cast<float>(envelope_types[sig_env_type_gate][s][3]);
            /* update item index */
            index++;
        }
    }
}

/* DECAY type initialization */
void env_engine::init_decay(const uint32_t _envId)
{
    /* provide (rendering) item index and object reference */
    uint32_t index;
    env_head* obj = &m_head[_envId];
    /* initialize IDLE segment */
    obj->m_state[0] = 0;
    obj->m_next[0] = 0;
    /* initialize decay start and stop indices (although stop isn't needed) */
    obj->m_startIndex = 1;
    obj->m_stopIndex = 0;
    /* outer loop - envelope (non-idle) segment initialization */
    for(uint32_t s = 0; s < sig_number_of_env_segments; s++)
    {
        /* segment definition */
        obj->m_state[s + 1] = envelope_types[sig_env_type_decay][s][0];
        obj->m_next[s + 1] = envelope_types[sig_env_type_decay][s][1];
        /* get (rendering) item index */
        index = obj->m_index;
        /* inner loop - initialize (rendering) items */
        for(uint32_t i = 0; i < obj->m_size; i++)
        {
            /* provide item reference */
            env_body* item = &m_body[index];
            /* item segment definition */
            item->m_segment[s + 1].m_dx = static_cast<float>(envelope_types[sig_env_type_decay][s][2]);
            item->m_segment[s + 1].m_dest = static_cast<float>(envelope_types[sig_env_type_decay][s][3]);
            /* update item index */
            index++;
        }
    }
}

/* envelope segment manipulation */
void env_engine::setSegmentDx(const uint32_t _voiceId, const uint32_t _envId, const uint32_t _segmentId, const float _value)
{
    /* update particular segment dx of selected envelope at specific voice (no reference for one-liner) */
    m_body[m_head[_envId].m_index + _voiceId].m_segment[_segmentId].m_dx = _value;
}

void env_engine::setSegmentDest(const uint32_t _voiceId, const uint32_t _envId, const uint32_t _segmentId, const float _value)
{
    /* update particular segment destination of selected envelope at specific voice (no reference for one-liner) */
    m_body[m_head[_envId].m_index + _voiceId].m_segment[_segmentId].m_dest = _value;
}

/* main tick functions (rendering) */
void env_engine::tickMono()
{
    /* for all mono envelopes: perform tick */
    for(uint32_t e = 0; e < m_polyIds.m_data[0].m_length; e++)
    {
        tickItem(0, m_polyIds.m_data[0].m_data[e]);
    }
}

void env_engine::tickPoly(const uint32_t _voiceId)
{
    /* for all poly envelopes: perform tick */
    for(uint32_t e = 0; e < m_polyIds.m_data[1].m_length; e++)
    {
        tickItem(_voiceId, m_polyIds.m_data[1].m_data[e]);
    }
}

/* basic tick function (basic rendering) */
void env_engine::tickItem(const uint32_t _voiceId, const uint32_t _envId)
{
    /* provide object and item references */
    env_head* obj = &m_head[_envId];
    env_body* item = &m_body[obj->m_index + _voiceId];
    /* provide segment position and updated segment difference (as changes could have occured) */
    const uint32_t pos = item->m_index;
    const float diff = item->m_segment[pos].m_dest - item->m_start;
    /* rendering mechanism - based on current state */
    switch(item->m_state)
    {
    case 0:
        /* IDLE */
        break;
    case 1:
        /* linear rendering */
        if(item->m_x < 1)
        {
            /* update signal and x */
            item->m_signal = item->m_start + (diff * item->m_x);
            item->m_x += item->m_segment[pos].m_dx;
        }
        else {
            /* clip x and update signal (could be further simplified, signal = dest) */
            item->m_x = 1;
            item->m_signal = item->m_start + diff;
            /* trigger next segment */
            nextSegment(_voiceId, _envId);
        }
        break;
    case 2:
        /* exponential rendering - decay2 mode */
        if(item->m_y > dsp_render_min)
        {
            /* update signal and y */
            item->m_signal = item->m_start + (diff * (1 - item->m_y));
            item->m_y *= 1 - item->m_segment[pos].m_dx;
        }
        else {
            /* keep forming the signal - excluding transition function - sustain can be changed in edit time */
            item->m_signal = item->m_start + diff;
        }
        break;
    case 3:
        /* polynomial rendering */
        if(item->m_x < 1)
        {
            /* apply curvature (16th order polynomial) */
            item->m_y = squaredCurvature(item->m_x, obj->m_attackCurve);    // 2nd power
            item->m_y = squaredCurvature(item->m_y, obj->m_attackCurve);    // 4th power
            item->m_y = squaredCurvature(item->m_y, obj->m_attackCurve);    // 8th power
            item->m_y = squaredCurvature(item->m_y, obj->m_attackCurve);    // 16th power
            /* update signal and x */
            item->m_signal = item->m_start + (diff * item->m_y);
            item->m_x += item->m_segment[pos].m_dx;
        }
        else {
            /* clip x and update signal (could be further simplified, signal = dest) */
            item->m_x = 1;
            item->m_signal = item->m_start + diff;
            /* trigger next segment */
            nextSegment(_voiceId, _envId);
        }
        break;
    case 4:
        /* exponential rendering - release mode */
        if(item->m_y > dsp_render_min)
        {
            /* update signal and y */
            item->m_signal = item->m_start + (diff * (1 - item->m_y));
            item->m_y *= 1 - item->m_segment[pos].m_dx;
        }
        else {
            /* null y, update signal */
            item->m_y = 0;
            item->m_signal = item->m_start + diff;
            /* next segment */
            nextSegment(_voiceId, _envId);
        }
        break;
    }
}

/* envelope segment traversal */
void env_engine::nextSegment(const uint32_t _voiceId, const uint32_t _envId)
{
    /* provide object and item references */
    env_head* obj = &m_head[_envId];
    env_body* item = &m_body[obj->m_index + _voiceId];
    /* update rendering variables */
    item->m_x = item->m_segment[item->m_next].m_dx;
    item->m_y = 1 - item->m_segment[item->m_next].m_dx;
    item->m_start = item->m_signal;
    /* update segment/state variables */
    item->m_state = obj->m_state[item->m_next];
    item->m_index = item->m_next;
    item->m_next = obj->m_next[item->m_next];
}

/* polynomial segment rendering */
float env_engine::squaredCurvature(const float _value, const float _curvature)
{
    /* depending on the curvature [-1 ... 1], values [-1 ... 1] are transformed into a 2nd order polynomial (sign sensitive) */
    return(_value * (1 + (_curvature * (std::abs(_value) - 1))));
}

/* envelope triggers */
void env_engine::startEnvelope(const uint32_t _voiceId, const uint32_t _envId, const float _attackCurve, const float _retriggerHardness)
{
    /* provide object and item references */
    env_head* obj = &m_head[_envId];
    env_body* item = &m_body[obj->m_index + _voiceId];
    /* update attack curvature */
    obj->m_attackCurve = _attackCurve;
    /* update rendering variables */
    item->m_x = item->m_segment[obj->m_startIndex].m_dx;
    item->m_y = 1 - item->m_segment[obj->m_startIndex].m_dx;
    item->m_start = (1.f - _retriggerHardness) * item->m_signal;
    /* update segment/state variables */
    item->m_state = obj->m_state[obj->m_startIndex];
    item->m_index = obj->m_startIndex;
    item->m_next = obj->m_next[obj->m_startIndex];
}

void env_engine::stopEnvelope(const uint32_t _voiceId, const uint32_t _envId)
{
    /* provide object and item references */
    env_head* obj = &m_head[_envId];
    env_body* item = &m_body[obj->m_index + _voiceId];
    /* update rendering variables */
    item->m_x = item->m_segment[obj->m_stopIndex].m_dx;
    item->m_y = 1 - item->m_segment[obj->m_stopIndex].m_dx;
    item->m_start = item->m_signal;
    /* update segment/state variables */
    item->m_state = obj->m_state[obj->m_stopIndex];
    item->m_index = obj->m_stopIndex;
    item->m_next = obj->m_next[obj->m_stopIndex];
}
