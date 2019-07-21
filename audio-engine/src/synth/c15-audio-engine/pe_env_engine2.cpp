/*****************************************************************************/
/** @file       pe_env_engine2.cpp
 *  @date       2018-06-27
 *  @version    0.1
 *  @author     Matthias Seeber
 *  @brief      All required C15 Envelope objects, which are contained in
 *              the main env_engine2 object.
******************************************************************************/

#include "pe_env_engine2.h"

/********************************************** Split Envelope Object Functionality **********************************************/

/*****************************************************************************/
/** @brief      trigger function for key down events
 *
 * (re-)starts the envelope on its first segment
 *
 *  @param      voiceId, current voice number relating to the key down event
******************************************************************************/

void env_object_adbdsr_split::start(const uint32_t _voiceId)
{
    /* set the body reference according to the current voice */

    env_body_split* body = &m_body[_voiceId];                           // a temporary body variable is declared

    /* prepare rendering variables for the imminent transition */

    body->m_x = m_segment[m_startIndex].m_dx[_voiceId];                 // x represents the transition progress [0 ... 1] for linear and polynomial transitions (x = dx)
    body->m_y = 1.f - m_segment[m_startIndex].m_dx[_voiceId];           // y represents the transition progress [1 ... 0] for exponential transitions (y = 1 - dx)
    body->m_start_magnitude = body->m_signal_magnitude;                 // the signal startpoint is updated for the magnitude signal (startpoint = signal)
    body->m_start_timbre = body->m_signal_timbre;                       // the signal startpoint is updated for the timbre signal (startpoint = signal)

    /* prepare state variables for the imminent transition */

    body->m_state = m_segment[m_startIndex].m_state;                    // the rendering state is updated according to the first segment (state = polynomial)
    body->m_index = m_startIndex;                                       // the segment index is updated to the first segment (index = 1)
    body->m_next = m_segment[m_startIndex].m_next;                      // the subsequent segment index is updated according to the first segment (next = linear)
}

/*****************************************************************************/
/** @brief      trigger function for key up events
 *
 * (re-)starts the envelope on its final segment
 *
 *  @param      voiceId, current voice number relating to the key up event
******************************************************************************/

void env_object_adbdsr_split::stop(const uint32_t _voiceId)
{
    /* set the body reference according to the current voice */

    env_body_split* body = &m_body[_voiceId];                           // a temporary body variable is declared

    /* prepare rendering variables for the imminent transition */

    body->m_x = m_segment[m_stopIndex].m_dx[_voiceId];                  // x represents the transition progress [0 ... 1] for linear and polynomial transitions (x = dx)
    body->m_y = 1.f - m_segment[m_stopIndex].m_dx[_voiceId];            // y represents the transition progress [1 ... 0] for exponential transitions (y = 1 - dx)
    body->m_start_magnitude = body->m_signal_magnitude;                 // the signal startpoint is updated for the magnitude signal (startpoint = signal)
    body->m_start_timbre = body->m_signal_timbre;                       // the signal startpoint is updated for the timbre signal (startpoint = signal)

    /* prepare state variables for the imminent transition */

    body->m_state = m_segment[m_stopIndex].m_state;                     // the rendering state is updated according to the final segment (state = exponential)
    body->m_index = m_stopIndex;                                        // the segment index is updated to the final segment (index = 4)
    body->m_next = m_segment[m_stopIndex].m_next;                       // the subsequent segment index is updated according to the final segment (next = idle)
}

/*****************************************************************************/
/** @brief      sampling function for clock ticks
 *
 * performing the transition rendering of the current segment
 *
 *  @param      voiceId, current voice number
******************************************************************************/

void env_object_adbdsr_split::tick(const uint32_t _voiceId)
{
    /* set the body and segment reference according to the current voice and body state */

    env_body_split* body = &m_body[_voiceId];                                                               // a temporary body variable is declared
    const uint32_t segment = body->m_index;                                                                 // a temporary segment index variable is declared

    /* calculate the current differences (signal to destination) */

    const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;   // current transition difference for the magnitude signal
    const float diff_timbre = m_segment[segment].m_dest_timbre[_voiceId] - body->m_start_timbre;            // current transition difference for the timbre signal
                                                                                                            // (difference = destination - startpoint) (magnitude, timbre)

    /* render according to the current state */

    switch(body->m_state)                                                                                   // (basic rendering instructions)
    {

    case 0:     /* idle - no rendering */

        break;

    case 1:     /* linear rendering (decay1 phase) - until transition is finished */

        if(body->m_x < 1.f)                                                                                 // (if(x < 1))
        {
            /* while transition is unfinished, the signals are formed by the current transition progress */

            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);              // (signal = startpoint + (difference * x)) (magnitude)
            body->m_signal_timbre = body->m_start_timbre + (diff_timbre * body->m_x);                       // (signal = startpoint + (difference * x)) (timbre)

            /* update the transition progress for the next clock tick */

            body->m_x += m_segment[segment].m_dx[_voiceId];                                                 // (x += dx)
        }
        else
        {
            /* when transition is finished, the signals are set to the corresponding segment destinations */

            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];                       // (signal = destination) (magnitude)
            body->m_signal_timbre = m_segment[segment].m_dest_timbre[_voiceId];                             // (signal = destination) (timbre)

            /* issue subsequent segment */

            nextSegment(_voiceId);
        }
        break;

    case 2:     /* exponential, quasi-infinite rendering (decay2 phase) - until transition is finished */

        if(body->m_y > dsp_render_min)                                                                      // (if(y > 1e-9))
        {
            /* while transition is unfinished, the signals are formed by the current transition progress */

            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));      // (signal = startpoint + (difference * (1 - y))) (magnitude)
            body->m_signal_timbre = body->m_start_timbre + (diff_timbre * (1.f - body->m_y));               // (signal = startpoint + (difference * (1 - y))) (timbre)

            /* update the transition progress for the next clock tick */

            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];                                           // (y *= 1 - dx)
        }
        else
        {
            /* when transition is finished, the signals are formed without the transition progress (but still according to the segment destinations) */

            body->m_signal_magnitude = body->m_start_magnitude + diff_magnitude;                            // (signal = startpoint + difference) (magnitude)
            body->m_signal_timbre = body->m_start_timbre + diff_timbre;                                     // (signal = startpoint + difference) (timbre)

            /* no subsequent segment will be issued (until a key down happens and triggers the final segment) */
        }
        break;

    case 3:     /* exponential, quasi-finite rendering (release phase) - until transition is finished */

        if(body->m_y > dsp_render_min)                                                                      // (if(y > 1e-9))
        {
            /* while transition is unfinished, the signals are formed by the current transition progress */

            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));      // (signal = startpoint + (difference * (1 - y))) (magnitude)
            body->m_signal_timbre = body->m_start_timbre + (diff_timbre * (1.f - body->m_y));               // (signal = startpoint + (difference * (1 - y))) (timbre)

            /* update the transition progress for the next clock tick */

            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];                                           // (y *= 1 - dx)
        }
        else
        {
            /* when transition is finished, the signals are set to the corresponding segment destinations */

            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];                       // (signal = destination) (magnitude)
            body->m_signal_timbre = m_segment[segment].m_dest_timbre[_voiceId];                             // (signal = destination) (timbre)

            /* issue subsequent segment */

            nextSegment(_voiceId);
        }
        break;

    case 4:     /* polynomial rendering (attack phase) - until transition is finished */

        if(body->m_x < 1.f)                                                                                 // (if(x < 1))
        {
            /* while transition is unfinished, the transition polynomial is formed according to the curvature */

            float x = NlToolbox::Curves::SquaredCurvature(body->m_x, m_segment[segment].m_curve);           // (polynomical curvature, 2nd degree)
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);                         // (polynomical curvature, 4th degree)
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);                         // (polynomical curvature, 8th degree)
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);                         // (polynomical curvature, 16th degree)

            /* the signals are formed by the current transition progress (more precise: the polynomial formed by the progress) */

            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * x);                      // (signal = startpoint + (difference * f(x))) (magnitude)
            body->m_signal_timbre = body->m_start_timbre + (diff_timbre * x);                               // (signal = startpoint + (difference * f(x))) (timbre)

            /* update the transition progress for the next clock tick */

            body->m_x += m_segment[segment].m_dx[_voiceId];                                                 // (x += dx)
        }
        else
        {
            /* when transition is finished, the signals are set to the corresponding segment destinations */

            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];                       // (signal = destination) (magnitude)
            body->m_signal_timbre = m_segment[segment].m_dest_timbre[_voiceId];                             // (signal = destination) (timbre)

            /* issue subsequent segment */

            nextSegment(_voiceId);
        }
        break;
    }
}

/*****************************************************************************/
/** @brief      trigger function for finished transitions
 *
 * (re-)starts the envelope on the corresponding subsequent segment
 * (function will be called automatically when a transition is finished)
 *
 *  @param      voiceId, current voice number
******************************************************************************/

void env_object_adbdsr_split::nextSegment(const uint32_t _voiceId)
{
    /* set the body reference according to the current voice */

    env_body_split* body = &m_body[_voiceId];                           // a temporary body variable is declared

    /* prepare rendering variables for the imminent transition */

    body->m_x = m_segment[body->m_next].m_dx[_voiceId];                 // x represents the transition progress [0 ... 1] for linear and polynomial transitions
    body->m_y = 1.f - m_segment[body->m_next].m_dx[_voiceId];           // y represents the transition progress [1 ... 0] for exponential transitions
    body->m_start_magnitude = body->m_signal_magnitude;                 // the signal startpoint is updated for the magnitude signal
    body->m_start_timbre = body->m_signal_timbre;                       // the signal startpoint is updated for the timbre signal

    /* prepare state variables for the imminent transition */

    body->m_state = m_segment[body->m_next].m_state;                    // the rendering state is updated according to the first segment
    body->m_index = body->m_next;                                       // the segment index is updated to the first segment
    body->m_next = m_segment[body->m_index].m_next;                     // the subsequent segment index is updated according to the first segment
}

/*****************************************************************************/
/** @brief      setter function for segment times
 *
 * updates the transition time of a specific segment within a specific voice
 *
 *  @param      voiceId, voice number specifying in which voice update should perform
 *  @param      segmentId, index specifying in which segment update should perform
 *  @param      value, the dx argument (representing the time)
******************************************************************************/

void env_object_adbdsr_split::setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
{
    /* access desired segment by segmentId, access dx in desired voice by voiceId, update by value */

    m_segment[_segmentId].m_dx[_voiceId] = _value;                      // dx [0 ... 1] represents the transition time (infinite ... zero)
}

/*****************************************************************************/
/** @brief      setter function for segment destinations
 *
 * updates the transition destinations of a specific segment within a specific voice
 *
 *  @param      voiceId, voice number specifying in which voice update should perform
 *  @param      segmentId, index specifying in which segment update should perform
 *  @param      splitMode, specifying if the segment destination should be splitted
 *  @param      value, the destination argument
******************************************************************************/

void env_object_adbdsr_split::setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const bool _splitMode, const float _value)
{
    /* updates destinations (for magnitude, timbre) according to splitMode (either splitted or unifomrly) */

    if(_splitMode)
    {
        /* splitMode update: destinations are determined according to split setting (by crossfades) - intended for breakpoint and sustain updates */

        m_segment[_segmentId].m_dest_magnitude[_voiceId] = NlToolbox::Crossfades::unipolarCrossFade(_value, m_peakLevels[_voiceId], m_splitValues[0]);
        m_segment[_segmentId].m_dest_timbre[_voiceId] = NlToolbox::Crossfades::unipolarCrossFade(_value, m_peakLevels[_voiceId], m_splitValues[1]);
    }
    else
    {
        /* uniform update: destinations are updated by value directly - intended for peak velocity */

        m_segment[_segmentId].m_dest_magnitude[_voiceId] = _value;
        m_segment[_segmentId].m_dest_timbre[_voiceId] = _value;
    }
}

/*****************************************************************************/
/** @brief      setter function for the split behavior
 *
 * causing either magnitude or timbre signal to be gate-like
 *
 *  @param      value, specifying the split behavior [-1 ... 1]
******************************************************************************/

void env_object_adbdsr_split::setSplitValue(const float _value)
{
    /* two separate crossfade values are determined */

    m_splitValues[0] = std::max(0.f, _value);           // magnitude xfade (param, 1)
    m_splitValues[1] = std::max(0.f, -_value);          // timbre xfade (param, 1)
}

/*****************************************************************************/
/** @brief      setter function for the attack curvature
 *
 * determines the change in slope depending on the curvature argument:
 *      curvature < 0 : decreasing slope
 *      curvature = 0 : constant slope
 *      curvature > 0 : increasing slope
 *
 *  @param      value, specifying the curvature [-1 ... 1]
******************************************************************************/

void env_object_adbdsr_split::setAttackCurve(const float _value)
{
    /* update the curvature of the attack segment */
    m_segment[1].m_curve = _value;
}

/*****************************************************************************/
/** @brief      setter function for the current peak level
 *
 * the peak level is the maximum of an envelope curve, determined by level
 * velocity
 *
 *  @param      value, specifying the peak level [0 ... 1]
******************************************************************************/

void env_object_adbdsr_split::setPeakLevel(const uint32_t _voiceId, const float _value)
{
    /* update the peak level according to voiceId */
    m_peakLevels[_voiceId] = _value;
}

/********************************************** Retrigger Envelope Object Functionality **********************************************/

/* */
void env_object_adbdsr_retrig::start(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_startIndex].m_dx[_voiceId];
    body->m_start_magnitude = (1.f - m_retriggerHardness) * body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[m_startIndex].m_state;
    body->m_index = m_startIndex;
    body->m_next = m_segment[m_startIndex].m_next;
}

/* */
void env_object_adbdsr_retrig::stop(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_stopIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_stopIndex].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[m_stopIndex].m_state;
    body->m_index = m_stopIndex;
    body->m_next = m_segment[m_stopIndex].m_next;
}

/* */
void env_object_adbdsr_retrig::tick(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    const uint32_t segment = body->m_index;
    const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;
    /* */
    switch(body->m_state)
    {

    case 0:     /* idle */

        break;

    case 1:     /* linear rendering (decay1 phase) */

        if(body->m_x < 1.f)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
            /* */
            body->m_x += m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;

    case 2:     /* exponential, quasi-infinite rendering (decay2 phase) */

        if(body->m_y > dsp_render_min)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));
            /* */
            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + diff_magnitude;
        }
        break;

    case 3:     /* exponential, quasi-finite rendering (release phase) */

        if(body->m_y > dsp_render_min)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));
            /* */
            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;

    case 4:     /* polynomial rendering (attack phase) */

        if(body->m_x < 1.f)
        {
            /* */
            float x = NlToolbox::Curves::SquaredCurvature(body->m_x, m_segment[segment].m_curve);
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
            x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * x);
            /* */
            body->m_x += m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    }
}

/* */
void env_object_adbdsr_retrig::nextSegment(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[body->m_next].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[body->m_next].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[body->m_next].m_state;
    body->m_index = body->m_next;
    body->m_next = m_segment[body->m_index].m_next;
}

/* */
void env_object_adbdsr_retrig::setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
{
    /* */
    m_segment[_segmentId].m_dx[_voiceId] = _value;
}

/* */
void env_object_adbdsr_retrig::setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
{
    /* */
    m_segment[_segmentId].m_dest_magnitude[_voiceId] = _value;
}

/* */
void env_object_adbdsr_retrig::setAttackCurve(const float _value)
{
    /* update the curvature of the attack segment */
    m_segment[1].m_curve = _value;
}

/* */
void env_object_adbdsr_retrig::setRetriggerHardness(const float _value)
{
    m_retriggerHardness = _value;
}

/********************************************** Gate Envelope Object Functionality **********************************************/

/* */
void env_object_gate::start(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_startIndex].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[m_startIndex].m_state;
    body->m_index = m_startIndex;
    body->m_next = m_segment[m_startIndex].m_next;
}

/* */
void env_object_gate::stop(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_stopIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_stopIndex].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[m_stopIndex].m_state;
    body->m_index = m_stopIndex;
    body->m_next = m_segment[m_stopIndex].m_next;
}

/* */
void env_object_gate::tick(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    const uint32_t segment = body->m_index;
    const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;
    /* */
    switch(body->m_state)
    {

    case 0:     /* idle */

        break;

    case 1:     /* linear rendering (attack phase) */

        if(body->m_x < 1.f)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
            /* */
            body->m_x += m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;

    case 2:     /* exponential, quasi-finite rendering (release phase) */

        if(body->m_y > dsp_render_min)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));
            /* */
            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    }
}

/* */
void env_object_gate::nextSegment(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[body->m_next].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[body->m_next].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[body->m_next].m_state;
    body->m_index = body->m_next;
    body->m_next = m_segment[body->m_index].m_next;
}

/* */
void env_object_gate::setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
{
    /* */
    m_segment[_segmentId].m_dx[_voiceId] = _value;
}

/* */
void env_object_gate::setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
{
    /* */
    m_segment[_segmentId].m_dest_magnitude[_voiceId] = _value;
}

/********************************************** Decay Envelope Object Functionality **********************************************/

/* */
void env_object_decay::start(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[m_startIndex].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[m_startIndex].m_state;
    body->m_index = m_startIndex;
    body->m_next = m_segment[m_startIndex].m_next;
}

/* */
void env_object_decay::tick(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    const uint32_t segment = body->m_index;
    const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;
    /* */
    switch(body->m_state)
    {

    case 0:     /* idle */

        break;

    case 1:     /* linear rendering (attack phase) */

        if(body->m_x < 1.f)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
            /* */
            body->m_x += m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;

    case 2:     /* exponential, quasi-finite rendering (release phase) */

        if(body->m_y > dsp_render_min)
        {
            /* */
            body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.f - body->m_y));
            /* */
            body->m_y *= 1.f - m_segment[segment].m_dx[_voiceId];
        }
        else
        {
            /* */
            body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
            /* */
            nextSegment(_voiceId);
        }
        break;
    }
}

/* */
void env_object_decay::nextSegment(const uint32_t _voiceId)
{
    /* */
    env_body_single* body = &m_body[_voiceId];
    /* */
    body->m_x = m_segment[body->m_next].m_dx[_voiceId];
    body->m_y = 1.f - m_segment[body->m_next].m_dx[_voiceId];
    body->m_start_magnitude = body->m_signal_magnitude;
    /* */
    body->m_state = m_segment[body->m_next].m_state;
    body->m_index = body->m_next;
    body->m_next = m_segment[body->m_index].m_next;
}

/* */
void env_object_decay::setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
{
    /* */
    m_segment[_segmentId].m_dx[_voiceId] = _value;
}

/* */
void env_object_decay::setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
{
    /* */
    m_segment[_segmentId].m_dest_magnitude[_voiceId] = _value;
}

/********************************************** Envelope Engine Object Functionality **********************************************/

/* */
void env_engine2::init(const uint32_t _voices, const float _gateRelease)
{
    /* */
    uint32_t segment, s, v;
    /* */
    for(s = 1; s <= sig_number_of_env_segments; s++)
    {
        /* */
        segment = s - 1;
        /* */
        m_env_a.m_segment[s].m_state = envelope2_definition[0][segment][0];
        m_env_a.m_segment[s].m_next = envelope2_definition[0][segment][1];
        /* */
        m_env_b.m_segment[s].m_state = envelope2_definition[0][segment][0];
        m_env_b.m_segment[s].m_next = envelope2_definition[0][segment][1];
        /* */
        m_env_c.m_segment[s].m_state = envelope2_definition[0][segment][0];
        m_env_c.m_segment[s].m_next = envelope2_definition[0][segment][1];
        /* */
        m_env_g.m_segment[s].m_state = envelope2_definition[1][segment][0];
        m_env_g.m_segment[s].m_next = envelope2_definition[1][segment][1];
        /* */
        m_env_f.m_segment[s].m_state = envelope2_definition[2][segment][0];
        m_env_f.m_segment[s].m_next = envelope2_definition[2][segment][1];
        m_env_f.setSegmentDx(0, s, static_cast<float>(envelope2_definition[2][segment][2]));
        m_env_f.setSegmentDest(0, s, static_cast<float>(envelope2_definition[2][segment][3]));
        /* */
        for(v = 0; v < _voices; v++)
        {
            /* */
            m_env_a.setSegmentDx(v, s, static_cast<float>(envelope2_definition[0][segment][2]));
            m_env_a.setSegmentDest(v, s, false, static_cast<float>(envelope2_definition[0][segment][3]));
            /* */
            m_env_b.setSegmentDx(v, s, static_cast<float>(envelope2_definition[0][segment][2]));
            m_env_b.setSegmentDest(v, s, false, static_cast<float>(envelope2_definition[0][segment][3]));
            /* */
            m_env_c.setSegmentDx(v, s, static_cast<float>(envelope2_definition[0][segment][2]));
            m_env_c.setSegmentDest(v, s, static_cast<float>(envelope2_definition[0][segment][3]));
            /* */
            m_env_g.setSegmentDx(v, s, static_cast<float>(envelope2_definition[1][segment][2]));
            m_env_g.setSegmentDest(v, s, static_cast<float>(envelope2_definition[1][segment][3]));
        }
    }
    /* */
    for(v = 0; v < _voices; v++)
    {
        /* */
        m_env_g.setSegmentDx(v, 2, _gateRelease);
    }
}

/* */
void env_engine2::tickMono()
{
    /* */
    m_env_f.tick(0);
}

/* */
void env_engine2::tickPoly(const uint32_t _voiceId)
{
    /* */
    m_env_a.tick(_voiceId);
    m_env_b.tick(_voiceId);
    m_env_c.tick(_voiceId);
    m_env_g.tick(_voiceId);
}
