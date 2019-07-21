#include <math.h>
#include "pe_exponentiator.h"

/* proper init */
void exponentiator::init()
{
    /* helper variables for initialization */
    uint32_t i;                                     // array index variable
    float exp;                                      // exponent variable
    /* construct linear pitch table */              // note: table construction loop goes up to range element (as tables are set to be range+1 in size)
    for(i = 0; i <= dsp_expon_lin_pitch_range; i++)
    {
        /* determine exponent of equation */
        exp = (static_cast<float>(i) + m_linear_pitch_from - m_freqExponent_offset) * m_scaleFreqExponent;
        /* determine power (base ^ exponent) */
        m_linear_pitch_table[i] = pow(m_freqBase, exp);
    }
    /* construct oscillator pitch table */
    m_oscillator_pitch_table[0] = 0.f;              // set first table element to zero
    /* construct hyperfloor part of oscillator pitch table (second till 19th element) */
    for(i = 1; i < 20; i++)
    {
        /* determine exponent of equation */
        exp = (hyperfloor(static_cast<float>(i) + m_oscillator_pitch_from) - m_freqExponent_offset) * m_scaleFreqExponent;
        /* determine power (base ^ exponent) */
        m_oscillator_pitch_table[i] = pow(m_freqBase, exp);
    }
    /* construct remaining (linear) part of oscillator pitch table */
    for(i = 20; i <= dsp_expon_osc_pitch_range; i++)
    {
        /* determine exponent of equation */
        exp = (static_cast<float>(i) + m_oscillator_pitch_from - m_freqExponent_offset) * m_scaleFreqExponent;
        /* determine power (base ^ exponent) */
        m_oscillator_pitch_table[i] = pow(m_freqBase, exp);
    }
    /* construct level table */
    m_level_table[0] = 0.f;                         // set first table element to zero
    for(i = 1; i <= dsp_expon_level_range; i++)
    {
        /* determine exponent of equation */
        exp = (static_cast<float>(i) + m_level_from) * m_scaleGainExponent;
        /* determine power (base ^ exponent) */
        m_level_table[i] = pow(m_gainBase, exp);
    }
    /* construct time table */
    m_time_table[0] = 0.f;                          // set first table element to zero
    for(i = 1; i <= dsp_expon_time_range; i++)
    {
        /* determine exponent of equation */
        exp = (static_cast<float>(i) + m_time_from) * m_scaleGainExponent;
        /* determine power (base ^ exponent) */
        m_time_table[i] = pow(m_gainBase, exp);
    }
}

/* helper methods */
float exponentiator::hyperfloor(float _value)
{
    /* the hyperbolic floor function of oscillator pitches */
    return((m_hyperfloor[0] * _value) / (m_hyperfloor[1] + _value));
}

float exponentiator::clip(float _min, float _max, float _value)
{
    /* clipping of values to ensure that table access does not exceed predefined ranges */
    return std::clamp(_value, _min, _max);
}

void exponentiator::setTablePos(float _value)
{
    /* convert floating point table position into integer and fractional parts */
    m_position_step = static_cast<uint32_t>(floor(_value));
    m_position_fine = _value - static_cast<float>(m_position_step);
}

float exponentiator::getInterpolated(float _fade, float _first, float _second)
{
    /* linear interpolation of two consecutive table values (first, second) by linear crossfade (fade) */
    return(((1 - _fade) * _first) + (_fade * _second));
}

/* main, run-time conversion methods */

float exponentiator::eval_lin_pitch(float _value)
{
    /* clip and translate value, update table position */
    setTablePos(clip(m_linear_pitch_from, m_linear_pitch_to, _value) - m_linear_pitch_from);
    /* perform interpolation and return result */
    return getInterpolated(m_position_fine, m_linear_pitch_table[m_position_step], m_linear_pitch_table[m_position_step + 1]);
}

float exponentiator::eval_osc_pitch(float _value)
{
    /* clip and translate value, update table position */
    setTablePos(clip(m_oscillator_pitch_from, m_oscillator_pitch_to, _value) - m_oscillator_pitch_from);
    /* perform interpolation and return result */
    return getInterpolated(m_position_fine, m_oscillator_pitch_table[m_position_step], m_oscillator_pitch_table[m_position_step + 1]);
}

float exponentiator::eval_level(float _value)
{
    /* clip and translate value, update table position */
    setTablePos(clip(m_level_from, m_level_to, _value) - m_level_from);
    /* perform interpolation and return result */
    return getInterpolated(m_position_fine, m_level_table[m_position_step], m_level_table[m_position_step + 1]);
}

float exponentiator::eval_time(float _value)
{
    /* clip and translate value, update table position */
    setTablePos(clip(m_time_from, m_time_to, _value) - m_time_from);
    /* perform interpolation and return result */
    return getInterpolated(m_position_fine, m_time_table[m_position_step], m_time_table[m_position_step + 1]);
}
