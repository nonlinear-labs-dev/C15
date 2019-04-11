/******************************************************************************/
/** @file           paramengine.cpp
    @date           2018-03-20
    @version        0.1
    @author         Matthias Seeber & Anton Schmied[2018-01-23]
    @brief          Paramengine Class member and method definitions
*******************************************************************************/
#include <math.h>
#include "paramengine.h"
#include "io/Log.h"
#include "ParameterStorage.h"

std::ostream& operator<<(std::ostream& lhs, const param_body& rhs)
{
  lhs << rhs.m_state << std::endl;

  return lhs;
}

/* proper init */
void paramengine::init(uint32_t _sampleRate, uint32_t _voices)
{
  /* determine crucial variables */
  m_samplerate = _sampleRate;
  m_reciprocal_samplerate = 1.f / static_cast<float>(_sampleRate);      // keep reciprocal frequency (1 / sampleRate)
  m_millisecond = dsp_samples_to_ms * static_cast<float>(_sampleRate);  // one millisecond in samples
  m_nyquist_frequency = static_cast<float>(_sampleRate >> 1);           // nyquist frequency (50% sampleRate)
  m_routePolyphony[1] = _voices;                                        // overwrite polyphony
  m_timeFactors[0] = 0.f;                                               // time convertsion factor (sync types)
  m_timeFactors[1] = 1.f;                                               // time convertsion factor (audio types)
  m_timeFactors[2] = static_cast<float>(_sampleRate / dsp_clock_rates[0]);  // time convertsion factor (fast types)
  m_timeFactors[3] = static_cast<float>(_sampleRate / dsp_clock_rates[1]);  // time convertsion factor (slow types)
  //std::cout << "Time Factors: (" << m_timeFactors[0] << ", " << m_timeFactors[1] << ", " << m_timeFactors[2] << ", " << m_timeFactors[3] << ")" << std::endl;
  /* initialize components */
  m_convert.init();
  /* initialize control shapers */
  m_combDecayCurve.setCurve(0.f, 0.25, 1.f);  // initialize control shaper for the comb decay parameter
  m_svfLBH1Curve.setCurve(-1.f, -1.f, 1.f);   // initialize control shaper for the LBH parameter (upper crossmix)
  m_svfLBH2Curve.setCurve(-1.f, 1.f, 1.f);    // initialize control shaper for the LBH parameter (lower crossmix)
  m_svfResonanceCurve.setCurve(0.f, 0.49f, 0.79f,
                               0.94f);  // initialize control shaper for the svf resonance parameter (later, test4)
  m_flaFeedbackCurve.setCurve(-1.f, -0.5f, 0.5f, 1.f);  // initialize control shaper for the flanger feedback curve
  m_flangerRateToDecay *= NlToolbox::Constants::twopi * m_reciprocal_samplerate;
  m_revColorCurve1.setCurve(66.f, 137.f, 130.f);  // initialize control shaper for the reverb internal lowpass (color)
  m_revColorCurve2.setCurve(29.f, 29.f, 85.f);    // initialize control shaper for the reverb internal highpass (color)

  /* initialize envelope events */
  for(uint32_t i = 0; i < sig_number_of_env_events; i++)
  {
    m_event.m_env[i].init();
  }
  /* First Run: initialize parameters by definition (see pe_defines_params.h) */
  for(uint32_t p = 0; p < sig_number_of_params; p++)
  {
    /* provide parameter reference */
    param_head* obj = &getHead(p);
    /* declarations according to parameter definition (index is determined in second run) */
    obj->m_clockType = static_cast<uint32_t>(param_definition[p][1]);  // clock type (sync/audio/fast/slow)
    obj->m_polyType = static_cast<uint32_t>(param_definition[p][2]);   // poly type (mono/poly)
    obj->m_size = m_routePolyphony[obj->m_polyType];                   // determine (rendering) size
    obj->m_normalize = 1.f / param_definition[p][3];                   // TCD range
    obj->m_scaleId = static_cast<uint32_t>(param_definition[p][4]);    // TCD scale id
    obj->m_scaleArg = param_definition[p][5];                          // TCD scale argument

    /* valid parameters are added to internal id lists, placeholders are ignored */
    if(param_definition[p][0] > -1)
    {
      obj->m_id = static_cast<uint32_t>(param_definition[p][0]);  // TCD id
      /* declare parameter according to clock type - crucial for rendering */
      m_parameters.addClockId(obj->m_clockType, obj->m_polyType, p);
      if(param_definition[p][6] > -1)
      {
        obj->m_postId = static_cast<uint32_t>(param_definition[p][6]);  // post processing id
        /* determine automatic post processing (copy, distribution) */
        m_parameters.addPostId(static_cast<uint32_t>(param_definition[p][7]), obj->m_clockType, obj->m_polyType, p);
      }
      else
      {
        obj->m_postId = 16383;
      }
    }
    else
    {
      obj->m_id = 16383;
    }
  }
  /* Second Run: set up ordered param_body indices */
  uint32_t index = 0;
  for(uint32_t clockType = 0; clockType < dsp_clock_types; clockType++)
  {
      for(auto &it : m_parameters.getClockIds(clockType, PARAM_MONO))
      {
          m_parameters.getHead(it).m_index = index;
          index += 1;
      }
  }
  for(uint32_t clockType = 0; clockType < dsp_clock_types; clockType++)
  {
      for(auto &it : m_parameters.getClockIds(clockType, PARAM_POLY))
      {
          m_parameters.getHead(it).m_index = index;
          index += _voices;
      }
  }
  /* initialize global utility parameters */
  for(uint32_t i = 0; i < sig_number_of_utilities; i++)
  {
    m_utilities[i].m_normalize = 1.f / utility_definition[i][0];
    m_utilities[i].m_scaleId = static_cast<uint32_t>(utility_definition[i][1]);
    m_utilities[i].m_scaleArg = utility_definition[i][2];
  }
  /* initialize envelopes */
  float gateRelease = 1.f / ((env_init_gateRelease * m_millisecond) + 1.f);
  /* initialize clip factors for envelope c */
  for(uint32_t i = 0; i < dsp_number_of_voices; i++)
  {
    m_env_c_clipFactor[i] = 1.f;
    m_note_shift[i] = 0.f;
  }
  /* initializing and testing new envelopes here... */
  m_new_envelopes.init(_voices, gateRelease);
  // initialize unison spread tables
  int32_t u_voice, u_index;
  for(uint32_t i = 0; i < dsp_number_of_voices; i++)
  {
    // prepare for unison voice
    u_voice = static_cast<int32_t>(i) + 1;
    // unison detune
    u_index = -(static_cast<int32_t>(i) >> 1);
    for(uint32_t p = 0; p <= i; p++)
    {
      m_unison_detune[i][p] = static_cast<float>(u_index);
      u_index += 1;
    }
    // unison phase and pan
    u_index = -static_cast<int32_t>(i);
    for(uint32_t p = 0; p <= i; p++)
    {
      m_unison_phase[i][p] = static_cast<float>(u_index) / static_cast<float>(2 * u_voice);
      m_unison_pan[i][p] = static_cast<float>(u_index) / static_cast<float>(i < 1 ? 1 : i);
      u_index += 2;
    }
    // zero padding
    for(uint32_t p = i + 1; p < dsp_number_of_voices; p++)
    {
      m_unison_detune[i][p] = 0.f;
      m_unison_phase[i][p] = 0.f;
      m_unison_pan[i][p] = 0.f;
    }
  }
  // print unison spread values - checked, values are correct
  /*
    std::cout << "Unison Spread Values" << std::endl;
    for(i = 0; i < dsp_number_of_voices; i++)
    {
        std::cout << "Voice:\t" << i + 1 << std::endl;
        std::cout << "\tDetune:\t";
        for(p = 0; p < dsp_number_of_voices; p++)
        {
            std::cout << m_unison_detune[i][p] << ", ";
        }
        std::cout << std::endl;
        std::cout << "\tPhase:\t";
        for(p = 0; p < dsp_number_of_voices; p++)
        {
            std::cout << m_unison_phase[i][p] << ", ";
        }
        std::cout << std::endl;
        std::cout << "\tPan:\t";
        for(p = 0; p < dsp_number_of_voices; p++)
        {
            std::cout << m_unison_pan[i][p] << ", ";
        }
        std::cout << std::endl;
    }
    */
  /* temporary testing */
  //testLevelVelocity();
  //testRounding();
}

/* helper - nyquist clip */
float paramengine::evalNyquist(float _freq)
{
  return std::min(m_nyquist_frequency, _freq);
}

/* TCD mechanism - scaling */
float paramengine::scale(const uint32_t _scaleId, const float _scaleArg, float _value)
{
  /* tcd scale methods (currently 13 established scalings), provide result variable */
  float result = 0.f;
  switch(_scaleId)
  {
    case 0:
      /* bipolar, linear scaling (argument is offset) */
      result = _value + _scaleArg;
      break;
    case 1:
      /* bipolar, linear scaling (argument is factor) */
      result = _value * _scaleArg;
      break;
    case 2:
      /* inverted, linear scaling (argument is offset) */
      result = _scaleArg - _value;
      break;
    case 3:
      /* bipolar, parabolic scaling (argument is offet) */
      result = (std::abs(_value) * _value) + _scaleArg;
      break;
    case 4:
      /* bipolar, parabolic scaling (argument is factor) */
      result = std::abs(_value) * _value * _scaleArg;
      break;
    case 5:
      /* bipolar, cubic scaling (argument is offset) */
      result = (_value * _value * _value) + _scaleArg;
      break;
    case 6:
      /* inverted, s-curve scaling (argument is offset) */
      _value = (2.f * (_scaleArg - _value)) - 1.f;
      result = (_value * _value * _value * -0.25f) + (0.75f * _value) + 0.5f;
      break;
    case 7:
      /* exponential, gain scaling (argument is offset) */
      result = m_convert.eval_level(_value + _scaleArg);
      break;
    case 8:
      /* exponential, osc-pitch scaling (argument is offset) */
      result = m_convert.eval_osc_pitch(_value + _scaleArg);  // includes hyperbolic floor, oscillators only
      break;
    case 9:
      /* exponential, lin-pitch scaling (argument is offset) */
      result = m_convert.eval_lin_pitch(_value + _scaleArg);  // other (linear) pitches
      break;
    case 10:
      /* exponential, drive scaling (argument is factor) */
      result = (m_convert.eval_level(_value) * _scaleArg) - _scaleArg;  // shaper a/b drives only
      break;
    case 11:
      /* exponential, mix-drive scaling (argument is factor) */
      result = m_convert.eval_level(_value) * _scaleArg;  // mixer shaper drives
      break;
    case 12:
      /* exponential, drive scaling (argument is offset) */
      result = m_convert.eval_level(_value + _scaleArg);  // cabinet drive and cab level
      break;
    case 13:
      /* exponential, envelope-time scaling (argument is offset) */
      result = m_convert.eval_time((_value * dsp_expon_time_factor) + _scaleArg);
      break;
  }
  /* return scaled value */
  return result;
}

/* TCD mechanism - time updates */
void paramengine::setDx(const uint32_t _voiceId, const uint32_t _paramId, float _value)
{
  /* provide object reference */
  param_head* obj = &getHead(_paramId);
  const uint32_t index = obj->m_index + _voiceId;
  /* handle by clock type and clip to fit [0 ... 1] range */
  _value = std::min(_value * m_timeFactors[obj->m_clockType], 1.f);
  /* pass value to (rendering) item */
  getBody(index).m_dx[0] = _value;
}

/* TCD mechanism - destination updates */
void paramengine::setDest(const uint32_t _voiceId, const uint32_t _paramId, float _value)
{
  /* provide object and (rendering) item references */
  param_head* obj = &getHead(_paramId);
  const uint32_t index = obj->m_index + _voiceId;
  param_body* item = &getBody(index);
  /* normalize and scale destination argument, pass result to (rendering) item */
  _value *= obj->m_normalize;
  item->m_dest = scale(obj->m_scaleId, obj->m_scaleArg, _value);
  /* apply according to preload and clock type */
  if(m_preload == 0)
  {
    /* sync type parameters apply directly, non-sync type parameters apply destinations */
    if(obj->m_clockType > 0)
    {
      applyDest(index);
    }
    else
    {
      applySync(index);
    }
  }
  else
  {
    item->m_preload++;
  }
}

/* TCD mechanism - preload functionality */
void paramengine::applyPreloaded(const uint32_t _voiceId, const uint32_t _paramId)
{
  /* provide object and (rendering) item references */
  param_head* obj = &getHead(_paramId);
  const uint32_t index = obj->m_index + _voiceId;
  param_body* item = &getBody(index);
  /* apply according to preload status */
  if(item->m_preload > 0)
  {
    item->m_preload = 0;
    /* sync type parameters apply directly, non-sync type parameters apply destinations */
    if(obj->m_clockType == 0)
    {
      applySync(index);
    }
    else
    {
      applyDest(index);
    }
  }
}

/* TCD mechanism - application (non-sync types performing transitions) */
void paramengine::applyDest(const uint32_t _index)
{
  /* provide (rendering) item reference */
  param_body* item = &getBody(_index);
  /* construct segment and set rendering state */
  item->m_start = item->m_signal;
  item->m_diff = item->m_dest - item->m_start;
  item->m_x = item->m_dx[1] = item->m_dx[0];
  item->m_state = 1;
}

/* TCD mechanism - application (sync types performing steps) */
void paramengine::applySync(const uint32_t _index)
{
  /* just update signal, no reference for one-liner */
  getBody(_index).m_signal = getBody(_index).m_dest;
}

/* parameter rendering */
void paramengine::tickItem(const uint32_t _index)
{
  /* provide (rendering) item reference */
  param_body* item = &getBody(_index);
  /* render when state is true */
  if(item->m_state == 1)
  {
    /* stop on final sample */
    if(item->m_x >= 1)
    {
      item->m_x = 1;
      item->m_state = 0;
    }
    /* update signal (and x) */
    item->m_signal = item->m_start + (item->m_diff * item->m_x);
    item->m_x += item->m_dx[1];
  }
}

/* TCD Key Events - keyDown */
void paramengine::keyDown(const uint32_t _voiceId, float _velocity)
{
  /* velocity scaling */
  _velocity = scale(m_utilities[0].m_scaleId, m_utilities[0].m_scaleArg, _velocity * m_utilities[0].m_normalize);
  /* update event, preload handling by parent (dsp_host) */
  m_event.m_mono.m_velocity = _velocity;            // set mono velocity (always last velocity)
  m_event.m_mono.m_type = 1;                        // set mono event type (1: DOWN)
  m_event.m_poly[_voiceId].m_velocity = _velocity;  // set poly velocity (according to voiceId)
  m_event.m_poly[_voiceId].m_type = 1;              // set poly event type (1: DOWN)
}

/* TCD Key Events - keyUp */
void paramengine::keyUp(const uint32_t _voiceId, float _velocity)
{
  /* velocity scaling */
  _velocity = scale(m_utilities[0].m_scaleId, m_utilities[0].m_scaleArg, _velocity * m_utilities[0].m_normalize);
  /* update event, preload handling by parent (dsp_host) */
  m_event.m_mono.m_velocity = _velocity;            // set mono velocity (always last velocity)
  m_event.m_mono.m_type = 0;                        // set mono event type (0: UP)
  m_event.m_poly[_voiceId].m_velocity = _velocity;  // set poly velocity (according to voiceId)
  m_event.m_poly[_voiceId].m_type = 0;              // set poly event type (0: UP)
}

/* TCD Key Events - preload functionality */
void paramengine::keyApply(const uint32_t _voiceId)
{
  if(m_event.m_poly[_voiceId].m_type == 1)
  {
    m_note_shift[_voiceId] = getSignal(P_MA_SH);
  }
  /* apply key event (update envelopes according to event type) */
#if test_milestone == 150
  const float pitch = getSignal(P_KEY_NP, _voiceId) + getSignal(P_MA_T) + m_note_shift[_voiceId];
#elif test_milestone == 155
  const uint32_t uVoice = static_cast<uint32_t>(getSignal(P_UN_V));
  const uint32_t uIndex = static_cast<uint32_t>(getSignal(P_KEY_IDX, _voiceId));
  const float pitch = getSignal(P_KEY_BP, _voiceId)
      + (getSignal(P_UN_DET) * m_unison_detune[uVoice][uIndex]) + getSignal(P_MA_T) + m_note_shift[_voiceId];
#elif test_milestone == 156
  const uint32_t uVoice = static_cast<uint32_t>(getSignal(P_UN_V));
  const uint32_t uIndex = m_unison_index[_voiceId];
  const float pitch = getSignal(P_KEY_BP, _voiceId) + (getSignal(P_UN_DET) * m_unison_detune[uVoice][uIndex])
      + getSignal(P_MA_T) + m_note_shift[_voiceId];
#endif
  const float velocity = m_event.m_poly[_voiceId].m_velocity;
  if(m_event.m_poly[_voiceId].m_type == 0)
  {
    /*
         *      "NEW" ENVELOPES:
         */
    /* key up */
    newEnvUpdateStop(_voiceId, pitch, velocity);  // all poly Envelopes at once
#if test_flanger_env_legato
    m_event.m_active--;  // track (decrease) number of active keys
#endif
  }
  else
  {
    /*
         *      "NEW" ENVELOPES:
         */
    /* key down */
    newEnvUpdateStart(_voiceId, pitch, velocity);
#if test_flanger_env_legato
    m_event.m_active++;  // track (increase) number of active keys
#endif
  }
}

/* TCD Key Events - mono key mechanism */
void paramengine::keyApplyMono()
{
  /*
     *      "NEW" ENVELOPES:
     */
#if test_flanger_env_legato == 0
  if(m_event.m_mono.m_type == 1)
  {
    m_new_envelopes.m_env_f.setSegmentDest(0, 1, m_event.m_mono.m_velocity);
    m_new_envelopes.m_env_f.start(0);
  }
#elif test_flanger_env_legato == 1
  if(m_event.m_mono.m_type == 1 && m_event.m_active == 0)
  {
    m_new_envelopes.m_env_f.setSegmentDest(0, 1, m_event.m_mono.m_velocity);
    m_new_envelopes.m_env_f.start(0);
  }
#endif
}
/*
 *      "NEW" ENVELOPES:
 */

/* */
void paramengine::newEnvUpdateStart(const uint32_t _voiceId, const float _pitch, const float _velocity)
{
  /* provide temporary variables */

  float time, timeKT, dest, levelVel, attackVel, decay1Vel, decay2Vel, levelKT, peak,
      unclipped;  // several variables are needed in order to determine envelope behavior

  /* envelope a update */

  timeKT = -0.5f * getSignal(P_EA_TKT) * _pitch;  // determine time key tracking according to pitch and parameter
  levelVel = -getSignal(P_EA_LV);                 // get level velocity parameter
  attackVel = -getSignal(P_EA_AV) * _velocity;    // determine attack velocity accorindg to velocity and parameter
  decay1Vel = -getSignal(P_EA_D1V) * _velocity;   // determine decay1 velocity accorindg to velocity and parameter
  decay2Vel = -getSignal(P_EA_D2V) * _velocity;   // determine decay2 velocity accorindg to velocity and parameter
  levelKT = getSignal(P_EA_LKT) * _pitch;         // determine level key tracking according to pitch and parameter
  peak = std::min(m_convert.eval_level(((1.f - _velocity) * levelVel) + levelKT),
                  env_clip_peak);  // determine peak level according to velocity and level parameters (max +3dB)

  m_event.m_env[0].m_levelFactor[_voiceId] = peak;  // remember peak level
  m_event.m_env[0].m_timeFactor[_voiceId][0] = m_convert.eval_level(timeKT + attackVel)
      * m_millisecond;  // determine time factor for attack segment (without actual attack time)
  m_event.m_env[0].m_timeFactor[_voiceId][1] = m_convert.eval_level(timeKT + decay1Vel)
      * m_millisecond;  // determine time factor for decay1 segment (without actual decay1 time)
  m_event.m_env[0].m_timeFactor[_voiceId][2] = m_convert.eval_level(timeKT + decay2Vel)
      * m_millisecond;  // determine time factor for decay2 segment (without actual decay2 time)

  m_new_envelopes.m_env_a.setSplitValue(getSignal(P_EA_SPL));  // update the split behavior by corresponding parameter
  m_new_envelopes.m_env_a.setAttackCurve(getSignal(P_EA_AC));  // update the attack curve by corresponding parameter
  m_new_envelopes.m_env_a.setPeakLevel(_voiceId,
                                       peak);  // update the current peak level (for magnitude/timbre crossfades)

  time = getSignal(P_EA_ATT)
      * m_event.m_env[0]
            .m_timeFactor[_voiceId][0];  // determine attack segment time according to time factor and parameter
  m_new_envelopes.m_env_a.setSegmentDx(_voiceId, 1, 1.f / (time + 1.f));  // update attack segment time
  m_new_envelopes.m_env_a.setSegmentDest(_voiceId, 1, false,
                                         peak);  // update attack segment destination (peak level) (no split behavior)

  // determine decay1 segment time according to time factor and parameter
  time = getSignal(P_EA_DEC1) * m_event.m_env[0].m_timeFactor[_voiceId][1];
  m_new_envelopes.m_env_a.setSegmentDx(_voiceId, 2, 1.f / (time + 1.f));  // update decay1 segment time
  // determine decay1 segment destination according to peak level and parameter
  dest = peak * getSignal(P_EA_BP);
  m_new_envelopes.m_env_a.setSegmentDest(_voiceId, 2, true,
                                         dest);  // update decay1 segment destination (split behavior)

  time = getSignal(P_EA_DEC2)
      * m_event.m_env[0]
            .m_timeFactor[_voiceId][2];  // determine decay2 segment time according to time factor and parameter
  m_new_envelopes.m_env_a.setSegmentDx(_voiceId, 3, 1.f / (time + 1.f));  // update decay2 segment time
  dest = peak * getSignal(P_EA_SUS);  // determine decay2 segment destination according to peak level and parameter
  m_new_envelopes.m_env_a.setSegmentDest(_voiceId, 3, true,
                                         dest);  // update decay2 segment destination (split behavior)

  /* envelope b update */

  timeKT = -0.5f * getSignal(P_EB_TKT) * _pitch;  // determine time key tracking according to pitch and parameter
  levelVel = -getSignal(P_EB_LV);                 // get level velocity parameter
  attackVel = -getSignal(P_EB_AV) * _velocity;    // determine attack velocity accorindg to velocity and parameter
  decay1Vel = -getSignal(P_EB_D1V) * _velocity;   // determine decay1 velocity accorindg to velocity and parameter
  decay2Vel = -getSignal(P_EB_D2V) * _velocity;   // determine decay2 velocity accorindg to velocity and parameter
  levelKT = getSignal(P_EB_LKT) * _pitch;         // determine level key tracking according to pitch and parameter
  peak = std::min(m_convert.eval_level(((1.f - _velocity) * levelVel) + levelKT),
                  env_clip_peak);  // determine peak level according to velocity and level parameters (max +3dB)

  m_event.m_env[1].m_levelFactor[_voiceId] = peak;  // remember peak level
  m_event.m_env[1].m_timeFactor[_voiceId][0] = m_convert.eval_level(timeKT + attackVel)
      * m_millisecond;  // determine time factor for attack segment (without actual attack time)
  m_event.m_env[1].m_timeFactor[_voiceId][1] = m_convert.eval_level(timeKT + decay1Vel)
      * m_millisecond;  // determine time factor for decay1 segment (without actual decay1 time)
  m_event.m_env[1].m_timeFactor[_voiceId][2] = m_convert.eval_level(timeKT + decay2Vel)
      * m_millisecond;  // determine time factor for decay2 segment (without actual decay2 time)

  m_new_envelopes.m_env_b.setSplitValue(getSignal(P_EB_SPL));  // update the split behavior by corresponding parameter
  m_new_envelopes.m_env_b.setAttackCurve(getSignal(P_EB_AC));  // update the attack curve by corresponding parameter
  m_new_envelopes.m_env_b.setPeakLevel(_voiceId,
                                       peak);  // update the current peak level (for magnitude/timbre crossfades)

  time = getSignal(P_EB_ATT)
      * m_event.m_env[1]
            .m_timeFactor[_voiceId][0];  // determine attack segment time according to time factor and parameter
  m_new_envelopes.m_env_b.setSegmentDx(_voiceId, 1, 1.f / (time + 1.f));  // update attack segment time
  m_new_envelopes.m_env_b.setSegmentDest(_voiceId, 1, false,
                                         peak);  // update attack segment destination (peak level) (no split behavior)

  time = getSignal(P_EB_DEC1)
      * m_event.m_env[1]
            .m_timeFactor[_voiceId][1];  // determine decay1 segment time according to time factor and parameter
  m_new_envelopes.m_env_b.setSegmentDx(_voiceId, 2, 1.f / (time + 1.f));  // update decay1 segment time
  dest = peak * getSignal(P_EB_BP);  // determine decay1 segment destination according to peak level and parameter
  m_new_envelopes.m_env_b.setSegmentDest(_voiceId, 2, true,
                                         dest);  // update decay1 segment destination (split behavior)

  time = getSignal(P_EB_DEC2)
      * m_event.m_env[1]
            .m_timeFactor[_voiceId][2];  // determine decay2 segment time according to time factor and parameter
  m_new_envelopes.m_env_b.setSegmentDx(_voiceId, 3, 1.f / (time + 1.f));  // update decay2 segment time
  dest = peak * getSignal(P_EB_SUS);  // determine decay2 segment destination according to peak level and parameter
  m_new_envelopes.m_env_b.setSegmentDest(_voiceId, 3, true,
                                         dest);  // update decay2 segment destination (split behavior)

  /* envelope c update */

  timeKT = -0.5f * getSignal(P_EC_TKT) * _pitch;  // determine time key tracking according to pitch and parameter
  levelVel = -getSignal(P_EC_LV);                 // get level velocity parameter
  attackVel = -getSignal(P_EC_AV) * _velocity;    // determine attack velocity accorindg to velocity and parameter
  levelKT = getSignal(P_EC_LKT) * _pitch;         // determine level key tracking according to pitch and parameter
  unclipped
      = m_convert.eval_level(((1.f - _velocity) * levelVel)
                             + levelKT);  // determine unclipped peak level according to velocity and level parameters
  peak = std::min(unclipped, env_clip_peak);  // determine clipped peak level (max +3dB)
  m_env_c_clipFactor[_voiceId]
      = unclipped / peak;  // determine unclipped / clipped peak factor in order to reconstruct unclipped signal later

  m_event.m_env[2].m_levelFactor[_voiceId] = peak;  // remember peak level
  m_event.m_env[2].m_timeFactor[_voiceId][0] = m_convert.eval_level(timeKT + attackVel)
      * m_millisecond;  // determine time factor for attack segment (without actual attack time)
  m_event.m_env[2].m_timeFactor[_voiceId][1] = m_convert.eval_level(timeKT)
      * m_millisecond;  // determine time factor for decay1 segment (without actual decay1 time)
  m_event.m_env[2].m_timeFactor[_voiceId][2]
      = m_event.m_env[2]
            .m_timeFactor[_voiceId][1];  // determine time factor for decay2 segment (without actual decay2 time)

  m_new_envelopes.m_env_c.setAttackCurve(getSignal(P_EC_AC));  // update the attack curve by corresponding parameter
  m_new_envelopes.m_env_c.setRetriggerHardness(
      getSignal(P_EC_RH));  // update the retrigger hardness by corresponding parameter

  time = getSignal(P_EC_ATT)
      * m_event.m_env[2]
            .m_timeFactor[_voiceId][0];  // determine attack segment time according to time factor and parameter
  m_new_envelopes.m_env_c.setSegmentDx(_voiceId, 1, 1.f / (time + 1.f));  // update attack segment time
  m_new_envelopes.m_env_c.setSegmentDest(_voiceId, 1,
                                         peak);  // update attack segment destination (peak level) (no split behavior)

  time = getSignal(P_EC_DEC1)
      * m_event.m_env[2]
            .m_timeFactor[_voiceId][1];  // determine decay1 segment time according to time factor and parameter
  m_new_envelopes.m_env_c.setSegmentDx(_voiceId, 2, 1.f / (time + 1.f));  // update decay1 segment time
  dest = peak * getSignal(P_EC_BP);  // determine decay1 segment destination according to peak level and parameter
  m_new_envelopes.m_env_c.setSegmentDest(_voiceId, 2, dest);  // update decay1 segment destination (no split behavior)

  time = getSignal(P_EC_DEC2)
      * m_event.m_env[2]
            .m_timeFactor[_voiceId][2];  // determine decay2 segment time according to time factor and parameter
  m_new_envelopes.m_env_c.setSegmentDx(_voiceId, 3, 1.f / (time + 1.f));  // update decay2 segment time
  dest = peak * getSignal(P_EC_SUS);  // determine decay2 segment destination according to peak level and parameter
  m_new_envelopes.m_env_c.setSegmentDest(_voiceId, 3, dest);  // update decay2 segment destination (no split behavior)

  /* start envelopes */

  m_new_envelopes.m_env_a.start(_voiceId);  // issue start of envelope a in current voice
  m_new_envelopes.m_env_b.start(_voiceId);  // issue start of envelope b in current voice
  m_new_envelopes.m_env_c.start(_voiceId);  // issue start of envelope c in current voice
  m_new_envelopes.m_env_g.start(_voiceId);  // issue start of envelope g in current voice
}

/* */
void paramengine::newEnvUpdateStop(const uint32_t _voiceId, const float _pitch, const float _velocity)
{
  /* provide temporary variables */

  float time, timeKT, releaseVel;  // several variables are needed in order to determine envelope behavior

  /* envelope a update */

  timeKT = -getSignal(P_EA_TKT) * _pitch;        // determine time key tracking according to pitch and parameter
  releaseVel = -getSignal(P_EA_RV) * _velocity;  // determine release velocity according to velocity and parameter

  m_event.m_env[0].m_timeFactor[_voiceId][3] = m_convert.eval_level(timeKT + releaseVel)
      * m_millisecond;  // determine time factor for release segment (without actual release time)

  if(getSignal(P_EA_REL) <= env_highest_finite_time)  // if the release time is meant to be finite (tcd: [0 ... 16000]):
  {
    /* finite release time */
    time = getSignal(P_EA_REL)
        * m_event.m_env[0]
              .m_timeFactor[_voiceId][3];  //      determine release segment time according to time factor and parameter
    m_new_envelopes.m_env_a.setSegmentDx(_voiceId, 4, 1.f / (time + 1.f));  //      update release segment time
  }
  else  // if the release time is meant to be infinite (tcd: [16001 ... 16160]):
  {
    /* infinite release time */
    m_new_envelopes.m_env_a.setSegmentDx(_voiceId, 4, 0.f);  //      update release segment time (set it to infinity)
  }

  /* envelope b update */

  timeKT = -getSignal(P_EB_TKT) * _pitch;        // determine time key tracking according to pitch and parameter
  releaseVel = -getSignal(P_EB_RV) * _velocity;  // determine release velocity according to velocity and parameter

  m_event.m_env[1].m_timeFactor[_voiceId][3] = m_convert.eval_level(timeKT + releaseVel)
      * m_millisecond;  // determine time factor for release segment (without actual release time)

  if(getSignal(P_EB_REL) <= env_highest_finite_time)  // if the release time is meant to be finite (tcd: [0 ... 16000]):
  {
    /* finite release time */
    time = getSignal(P_EB_REL)
        * m_event.m_env[1]
              .m_timeFactor[_voiceId][3];  //      determine release segment time according to time factor and parameter
    m_new_envelopes.m_env_b.setSegmentDx(_voiceId, 4, 1.f / (time + 1.f));  //      update release segment time
  }
  else  // if the release time is meant to be infinite (tcd: [16001 ... 16160]):
  {
    /* infinite release time */
    m_new_envelopes.m_env_b.setSegmentDx(_voiceId, 4, 0.f);  //      update release segment time (set it to infinity)
  }

  /* envelope c update */

  timeKT = -getSignal(P_EC_TKT) * _pitch;        // determine time key tracking according to pitch and parameter
  releaseVel = -getSignal(P_EC_RV) * _velocity;  // determine release velocity according to velocity and parameter

  m_event.m_env[2].m_timeFactor[_voiceId][3] = m_convert.eval_level(timeKT + releaseVel)
      * m_millisecond;  // determine time factor for release segment (without actual release time)

  if(getSignal(P_EC_REL) <= env_highest_finite_time)  // if the release time is meant to be finite (tcd: [0 ... 16000]):
  {
    /* finite release time */
    time = getSignal(P_EC_REL)
        * m_event.m_env[2]
              .m_timeFactor[_voiceId][3];  //      determine release segment time according to time factor and parameter
    m_new_envelopes.m_env_c.setSegmentDx(_voiceId, 4, 1.f / (time + 1.f));  //      update release segment time
  }
  else  // if the release time is meant to be infinite (tcd: [16001 ... 16160]):
  {
    /* infinite release time */
    m_new_envelopes.m_env_c.setSegmentDx(_voiceId, 4, 0.f);  //      update release segment time (set it to infinity)
  }

  /* stop envelopes */

  m_new_envelopes.m_env_a.stop(_voiceId);  // issue stop of envelope a in current voice
  m_new_envelopes.m_env_b.stop(_voiceId);  // issue stop of envelope b in current voice
  m_new_envelopes.m_env_c.stop(_voiceId);  // issue stop of envelope c in current voice
  m_new_envelopes.m_env_g.stop(_voiceId);  // issue stop of envelope g in current voice
}

/* */
void paramengine::newEnvUpdateTimes(const uint32_t _voiceId)
{
  /* provide temporary variables */

  float time;  // the time variable is needed in order to determine envelope behavior

  /* envelope a update */

  time = getSignal(P_EA_ATT)
      * m_event.m_env[0]
            .m_timeFactor[_voiceId][0];  // determine attack segment time according to time factor and parameter
  m_new_envelopes.m_env_a.setSegmentDx(_voiceId, 1, 1.f / (time + 1.f));  // update attack segment time

  time = getSignal(P_EA_DEC1)
      * m_event.m_env[0]
            .m_timeFactor[_voiceId][1];  // determine decay1 segment time according to time factor and parameter
  m_new_envelopes.m_env_a.setSegmentDx(_voiceId, 2, 1.f / (time + 1.f));  // update decay1 segment time

  time = getSignal(P_EA_DEC2)
      * m_event.m_env[0]
            .m_timeFactor[_voiceId][2];  // determine decay2 segment time according to time factor and parameter
  m_new_envelopes.m_env_a.setSegmentDx(_voiceId, 3, 1.f / (time + 1.f));  // update decay2 segment time

  if(getSignal(P_EA_REL) <= env_highest_finite_time)  // if the release time is meant to be finite (tcd: [0 ... 16000]):
  {
    /* finite release time */
    time = getSignal(P_EA_REL)
        * m_event.m_env[0]
              .m_timeFactor[_voiceId][3];  //      determine release segment time according to time factor and parameter
    m_new_envelopes.m_env_a.setSegmentDx(_voiceId, 4, 1.f / (time + 1.f));  //      update release segment time
  }
  else  // if the release time is meant to be infinite (tcd: [16001 ... 16160])
  {
    /* infinite release time */
    m_new_envelopes.m_env_a.setSegmentDx(_voiceId, 4, 0.f);  //      update release segment time (set it to infinity)
  }

  /* envelope b update */

  time = getSignal(P_EB_ATT)
      * m_event.m_env[1]
            .m_timeFactor[_voiceId][0];  // determine attack segment time according to time factor and parameter
  m_new_envelopes.m_env_b.setSegmentDx(_voiceId, 1, 1.f / (time + 1.f));  // update attack segment time

  time = getSignal(P_EB_DEC1)
      * m_event.m_env[1]
            .m_timeFactor[_voiceId][1];  // determine decay1 segment time according to time factor and parameter
  m_new_envelopes.m_env_b.setSegmentDx(_voiceId, 2, 1.f / (time + 1.f));  // update decay1 segment time

  time = getSignal(P_EB_DEC2)
      * m_event.m_env[1]
            .m_timeFactor[_voiceId][2];  // determine decay2 segment time according to time factor and parameter
  m_new_envelopes.m_env_b.setSegmentDx(_voiceId, 3, 1.f / (time + 1.f));  // update decay2 segment time

  if(getSignal(P_EB_REL) <= env_highest_finite_time)  // if the release time is meant to be finite (tcd: [0 ... 16000]):
  {
    /* finite release time */
    time = getSignal(P_EB_REL)
        * m_event.m_env[1]
              .m_timeFactor[_voiceId][3];  //      determine release segment time according to time factor and parameter
    m_new_envelopes.m_env_b.setSegmentDx(_voiceId, 4, 1.f / (time + 1.f));  //      update release segment time
  }
  else  // if the release time is meant to be infinite (tcd: [16001 ... 16160])
  {
    /* infinite release time */
    m_new_envelopes.m_env_b.setSegmentDx(_voiceId, 4, 0.f);  //      update release segment time (set it to infinity)
  }

  /* envelope c update */

  time = getSignal(P_EC_ATT)
      * m_event.m_env[2]
            .m_timeFactor[_voiceId][0];  // determine attack segment time according to time factor and parameter
  m_new_envelopes.m_env_c.setSegmentDx(_voiceId, 1, 1.f / (time + 1.f));  // update attack segment time

  time = getSignal(P_EC_DEC1)
      * m_event.m_env[2]
            .m_timeFactor[_voiceId][1];  // determine decay1 segment time according to time factor and parameter
  m_new_envelopes.m_env_c.setSegmentDx(_voiceId, 2, 1.f / (time + 1.f));  // update decay1 segment time

  time = getSignal(P_EC_DEC2)
      * m_event.m_env[2]
            .m_timeFactor[_voiceId][2];  // determine decay2 segment time according to time factor and parameter
  m_new_envelopes.m_env_c.setSegmentDx(_voiceId, 3, 1.f / (time + 1.f));  // update decay2 segment time

  if(getSignal(P_EC_REL) <= env_highest_finite_time)  // if the release time is meant to be finite (tcd: [0 ... 16000]):
  {
    /* finite release time */
    time = getSignal(P_EC_REL)
        * m_event.m_env[2]
              .m_timeFactor[_voiceId][3];  //      determine release segment time according to time factor and parameter
    m_new_envelopes.m_env_c.setSegmentDx(_voiceId, 4, 1.f / (time + 1.f));  //      update release segment time
  }
  else  // if the release time is meant to be infinite (tcd: [16001 ... 16160])
  {
    /* infinite release time */
    m_new_envelopes.m_env_c.setSegmentDx(_voiceId, 4, 0.f);  //      update release segment time (set it to infinity)
  }
}

/* */
void paramengine::newEnvUpdateLevels(const uint32_t _voiceId)
{
  /* provide temporary variables */

  float peak, dest;  // two variables are needed in order to determine envelope behavior

  /* envelope a update */

  peak = m_event.m_env[0].m_levelFactor[_voiceId];  // get envelope peak level (was determined by last key down)

  dest = peak * getSignal(P_EA_BP);  // determine decay1 segment destination according to peak level and parameter
  m_new_envelopes.m_env_a.setSegmentDest(_voiceId, 2, true,
                                         dest);  // update decay1 segment destination (split behavior)

  dest = peak * getSignal(P_EA_SUS);  // determine decay2 segment destination according to peak level and parameter
  m_new_envelopes.m_env_a.setSegmentDest(_voiceId, 3, true,
                                         dest);  // update decay2 segment destinatino (split behavior)

  /* envelope b update */

  peak = m_event.m_env[1].m_levelFactor[_voiceId];  // get envelope peak level (was determined by last key down)

  dest = peak * getSignal(P_EB_BP);  // determine decay1 segment destination according to peak level and parameter
  m_new_envelopes.m_env_b.setSegmentDest(_voiceId, 2, true,
                                         dest);  // update decay1 segment destination (split behavior)

  dest = peak * getSignal(P_EB_SUS);  // determine decay2 segment destination according to peak level and parameter
  m_new_envelopes.m_env_b.setSegmentDest(_voiceId, 3, true,
                                         dest);  // update decay2 segment destinatino (split behavior)

  /* envelope c update */

  peak = m_event.m_env[2].m_levelFactor[_voiceId];  // get envelope peak level (was determined by last key down)

  dest = peak * getSignal(P_EC_BP);  // determine decay1 segment destination according to peak level and parameter
  m_new_envelopes.m_env_c.setSegmentDest(_voiceId, 2, dest);  // update decay1 segment destination (no split behavior)

  dest = peak * getSignal(P_EC_SUS);  // determine decay2 segment destination according to peak level and parameter
  m_new_envelopes.m_env_c.setSegmentDest(_voiceId, 3, dest);  // update decay2 segment destinatino (no split behavior)
}

/* Poly Post Processing - slow parameters */
void paramengine::postProcessPoly_slow(ParameterStorage& params, const uint32_t _voiceId)
{
  /* automatic mono to poly distribution */
  for(auto &it : m_parameters.getPostIds(PARAM_SPREAD, PARAM_SLOW, PARAM_MONO))
  {
      auto p = getHead(it).m_postId;
      params[p] = getSignal(it);
  }
  /* automatic poly to poly copy - each voice */
  for(auto &it : m_parameters.getPostIds(PARAM_SINGLE, PARAM_SLOW, PARAM_POLY))
  {
      auto p = getHead(it).m_postId;
      params[p] = getSignal(it, _voiceId);
  }
  /*
     *      "NEW" ENVELOPES:
     */
  newEnvUpdateTimes(_voiceId);
  /* Pitch Updates */
#if test_milestone == 150
  const float notePitch
      = getSignal(P_KEY_NP, _voiceId) + getSignal(P_MA_T) + m_note_shift[_voiceId];
#elif test_milestone == 155
  const uint32_t uVoice = static_cast<uint32_t>(getSignal(P_UN_V));
  const uint32_t uIndex = static_cast<uint32_t>(getSignal(P_KEY_IDX, _voiceId));
  const float notePitch = getSignal(P_KEY_BP, _voiceId)
      + (getSignal(P_UN_DET) * m_unison_detune[uVoice][uIndex]) + getSignal(P_MA_T) + m_note_shift[_voiceId];
#elif test_milestone == 156
  const uint32_t uVoice = static_cast<uint32_t>(getSignal(P_UN_V));
  const uint32_t uIndex = m_unison_index[_voiceId];
  const float notePitch = getSignal(P_KEY_BP, _voiceId) + (getSignal(P_UN_DET) * m_unison_detune[uVoice][uIndex])
      + getSignal(P_MA_T) + m_note_shift[_voiceId];
#endif
  float keyTracking, unitPitch, envMod, unitSign, unitSpread, unitMod;
  /* Oscillator A */
  /* - Oscillator A Frequency in Hz (Base Pitch, Master Tune, Key Tracking, Osc Pitch, Envelope C) */
  keyTracking = getSignal(P_OA_PKT);
  unitPitch = getSignal(P_OA_P);
  envMod = params[ENV_C_UNCL] * getSignal(P_OA_PEC);
  params[OSC_A_FRQ] = evalNyquist(m_pitch_reference * unitPitch
                                  * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking) + envMod));
  /* - Oscillator A Fluctuation (Envelope C) */
  envMod = getSignal(P_OA_FEC);
  params[OSC_A_FLUEC] = getSignal(P_OA_F) * NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_C_CLIP], envMod);
  /* - Oscillator A Chirp Frequency in Hz */
  params[OSC_A_CHI] = evalNyquist(getSignal(P_OA_CHI) * 440.f);
  /* Oscillator B */
  /* - Oscillator B Frequency in Hz (Base Pitch, Master Tune, Key Tracking, Osc Pitch, Envelope C) */
  keyTracking = getSignal(P_OB_PKT);
  unitPitch = getSignal(P_OB_P);
  envMod = params[ENV_C_UNCL] * getSignal(P_OB_PEC);
  params[OSC_B_FRQ] = evalNyquist(m_pitch_reference * unitPitch
                                  * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking) + envMod));
  /* - Oscillator B Fluctuation (Envelope C) */
  envMod = getSignal(P_OB_FEC);
  params[OSC_B_FLUEC] = getSignal(P_OB_F) * NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_C_CLIP], envMod);
  /* - Oscillator B Chirp Frequency in Hz */
  params[OSC_B_CHI] = evalNyquist(getSignal(P_OB_CHI) * 440.f);
  /* Comb Filter */
  /* - Comb Filter Pitch as Frequency in Hz (Base Pitch, Master Tune, Key Tracking, Comb Pitch) */
  keyTracking = getSignal(P_CMB_PKT);
  unitPitch = getSignal(P_CMB_P);
  // as a tonal component, the reference tone frequency is applied (instead of const 440 Hz)
  params[CMB_FRQ]
      = evalNyquist(m_pitch_reference * unitPitch * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking)));
  /* - Comb Filter Bypass (according to Pitch parameter - without key tracking or reference freq) */
  params[CMB_BYP] = unitPitch > dsp_comb_max_freqFactor
      ? 1.f
      : 0.f;  // check for bypassing comb filter, max_freqFactor corresponds to Pitch of 119.99 ST
  /* - Comb Filter Decay Time (Base Pitch, Master Tune, Gate Env, Dec Time, Key Tracking, Gate Amount) */
  keyTracking = getSignal(P_CMB_DKT);
  unitSign = 0.001f * (getSignal(P_CMB_D) < 0 ? -1.f : 1.f);
#if test_comb_decay_gate_mode == 0
  // apply decay time directly
  envMod = 1.f - ((1.f - params[ENV_G_SIG]) * m_combDecayCurve.applyCurve(getSignal(P_CMB_DG)));
  unitPitch = (-0.5f * notePitch * keyTracking) + (std::abs(getSignal(P_CMB_D)) * envMod);
  params[CMB_DEC] = m_convert.eval_level(unitPitch) * unitSign;
#elif test_comb_decay_gate_mode == 1
  // determine decay times min, max before crossfading them by gate signal (audio post processing)
  envMod = 1.f - m_combDecayCurve.applyCurve(getSignal(P_CMB_DG));
  unitMod = std::abs(getSignal(P_CMB_D));
  unitPitch = (-0.5f * notePitch * keyTracking);
  m_comb_decay_times[0] = m_convert.eval_level(unitPitch + (unitMod * envMod)) * unitSign;
  m_comb_decay_times[1] = m_convert.eval_level(unitPitch + unitMod) * unitSign;
#endif
  /* - Comb Filter Allpass Frequency (Base Pitch, Master Tune, Key Tracking, AP Tune, Env C) */
  keyTracking = getSignal(P_CMB_APKT);
  unitPitch = getSignal(P_CMB_APT);
  envMod = params[ENV_C_UNCL] * getSignal(P_CMB_APEC);
  params[CMB_APF] = evalNyquist(440.f * unitPitch
                                * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking)
                                                           + envMod));  // not sure if APF needs Nyquist Clipping?
  //params[CMB_APF] = 440.f * unitPitch * m_convert.eval_lin_pitch(69.f + (basePitch * keyTracking) + envMod);                   // currently APF without Nyquist Clipping
  /* - Comb Filter Lowpass ('Hi Cut') Frequency (Base Pitch, Master Tune, Key Tracking, Hi Cut, Env C) */
  keyTracking = getSignal(P_CMB_LPKT);
  unitPitch = getSignal(P_CMB_LP);
  envMod = params[ENV_C_UNCL] * getSignal(P_CMB_LPEC);
  params[CMB_LPF] = evalNyquist(440.f * unitPitch
                                * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking)
                                                           + envMod));  // not sure if LPF needs Nyquist Clipping?
  //params[CMB_LPF] = 440.f * unitPitch * m_convert.eval_lin_pitch(69.f + (basePitch * keyTracking) + envMod);                   // currently LPF without Nyquist Clipping
  /* State Variable Filter */
  /* - Cutoff Frequencies */
  keyTracking = getSignal(P_SVF_CKT);                  // get Key Tracking
  envMod = params[ENV_C_UNCL] * getSignal(P_SVF_CEC);  // get Envelope C Modulation (amount * envelope_c_signal)
  // as a tonal component, the Reference Tone frequency is applied (instead of const 440 Hz)
  unitPitch = m_pitch_reference * getSignal(P_SVF_CUT);
  unitSpread = getSignal(P_SVF_SPR);  // get the Spread parameter (already scaled to 50%)
  unitMod = getSignal(P_SVF_FM);      // get the FM parameter
  /*   now, calculate the actual filter frequencies and put them in the shared signal array */
  params[SVF_F1_CUT] = evalNyquist(unitPitch
                                   * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking) + envMod
                                                              + unitSpread));  // SVF upper 2PF Cutoff Frequency
  params[SVF_F2_CUT] = evalNyquist(unitPitch
                                   * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking) + envMod
                                                              - unitSpread));  // SVF lower 2PF Cutoff Frequency
  params[SVF_F1_FM] = params[SVF_F1_CUT] * unitMod;                            // SVF upper 2PF FM Amount (Frequency)
  params[SVF_F2_FM] = params[SVF_F2_CUT] * unitMod;                            // SVF lower 2PF FM Amount (Frequency)
  /* - Resonance */
  keyTracking = getSignal(P_SVF_RKT) * m_svfResFactor;
  envMod = params[ENV_C_CLIP] * getSignal(P_SVF_REC);
  unitPitch = getSignal(P_SVF_RES) + envMod + (notePitch * keyTracking);
  //params[SVF_RES] = m_svfResonanceCurve.applyCurve(std::clamp(unitPitch, 0.f, 1.f));
  float res = 1.f
      - m_svfResonanceCurve.applyCurve(
            std::clamp(unitPitch, 0.f, 1.f));  // NEW resonance handling directly in post processing
  params[SVF_RES] = std::max(res + res, 0.02f);
  /* - Feedback Mixer */
  /*   - determine Highpass Filter Frequency */
  params[FBM_HPF] = evalNyquist(m_convert.eval_lin_pitch(12.f + notePitch) * 440.f);
}

/* Poly Post Processing - fast parameters */
void paramengine::postProcessPoly_fast(ParameterStorage& params, const uint32_t _voiceId)
{
  /* automatic mono to poly distribution */
  for(auto &it : m_parameters.getPostIds(PARAM_SPREAD, PARAM_FAST, PARAM_MONO))
  {
      auto p = getHead(it).m_postId;
      params[p] = getSignal(it);
  }
  /* automatic poly to poly copy - each voice */
  for(auto &it : m_parameters.getPostIds(PARAM_SINGLE, PARAM_FAST, PARAM_POLY))
  {
      auto p = getHead(it).m_postId;
      params[p] = getSignal(it, _voiceId);
  }
  /*
     *      "NEW" ENVELOPES:
     */
  newEnvUpdateLevels(_voiceId);
  /* temporary variables */
  float tmp_lvl, tmp_pan, tmp_abs;
#if test_milestone == 150
  const float notePitch
      = getSignal(P_KEY_NP, _voiceId) + getSignal(P_MA_T) + m_note_shift[_voiceId];
#elif test_milestone == 155
  const uint32_t uVoice = static_cast<uint32_t>(getSignal(P_UN_V));
  const uint32_t uIndex = static_cast<uint32_t>(getSignal(P_KEY_IDX, _voiceId));
  const float basePitch = getSignal(P_KEY_BP, _voiceId);
  const float notePitch = basePitch + (getSignal(P_UN_DET) * m_unison_detune[uVoice][uIndex]) + getSignal(P_MA_T)
      + m_note_shift[_voiceId];
#elif test_milestone == 156
  const uint32_t uVoice = static_cast<uint32_t>(getSignal(P_UN_V));
  const uint32_t uIndex = m_unison_index[_voiceId];
  const float basePitch = getSignal(P_KEY_BP, _voiceId);
  const float notePitch = basePitch + (getSignal(P_UN_DET) * m_unison_detune[uVoice][uIndex]) + getSignal(P_MA_T)
      + m_note_shift[_voiceId];
#endif
  /* State Variable Filter */
  /* - LBH */
  tmp_lvl = getSignal(P_SVF_LBH);
  params[SVF_LBH_1] = m_svfLBH1Curve.applyCurve(tmp_lvl);
  params[SVF_LBH_2] = m_svfLBH2Curve.applyCurve(tmp_lvl);
  /* - Parallel */
  tmp_lvl = getSignal(P_SVF_PAR);
  tmp_abs = std::abs(tmp_lvl);
  params[SVF_PAR_1] = 0.7f * tmp_abs;
  params[SVF_PAR_2] = (0.7f * tmp_lvl) + (1.f - tmp_abs);
  params[SVF_PAR_3] = 1.f - tmp_abs;
  params[SVF_PAR_4] = tmp_abs;
  /* Output Mixer */
#if test_milestone == 150
  const float poly_pan = getSignal(P_KEY_VP, _voiceId);
#elif test_milestone == 155
  const float poly_pan
      = (getSignal(P_OM_KP) * (basePitch - 6.f)) + (getSignal(P_UN_PAN) * m_unison_pan[uVoice][uIndex]);
#elif test_milestone == 156
  const float poly_pan
      = (getSignal(P_OM_KP) * (basePitch - 6.f)) + (getSignal(P_UN_PAN) * m_unison_pan[uVoice][uIndex]);
#endif
  /* - Branch A */
  tmp_lvl = getSignal(P_OM_AL);
  tmp_pan = std::clamp(getSignal(P_OM_AP) + poly_pan, 0.f, 1.f);
  params[OUT_A_L] = tmp_lvl * (1.f - tmp_pan);
  params[OUT_A_R] = tmp_lvl * tmp_pan;
  /* - Branch B */
  tmp_lvl = getSignal(P_OM_BL);
  tmp_pan = std::clamp(getSignal(P_OM_BP) + poly_pan, 0.f, 1.f);
  params[OUT_B_L] = tmp_lvl * (1.f - tmp_pan);
  params[OUT_B_R] = tmp_lvl * tmp_pan;
  /* - Comb Filter */
  tmp_lvl = getSignal(P_OM_CL);
  tmp_pan = std::clamp(getSignal(P_OM_CP) + poly_pan, 0.f, 1.f);
  params[OUT_CMB_L] = tmp_lvl * (1.f - tmp_pan);
  params[OUT_CMB_R] = tmp_lvl * tmp_pan;
  /* - State Variable Filter */
  tmp_lvl = getSignal(P_OM_SL);
  tmp_pan = std::clamp(getSignal(P_OM_SP) + poly_pan, 0.f, 1.f);
  params[OUT_SVF_L] = tmp_lvl * (1.f - tmp_pan);
  params[OUT_SVF_R] = tmp_lvl * tmp_pan;
  /* - Feedback Mixer */
  tmp_lvl = getSignal(P_FBM_LVL);
  tmp_pan = std::min(m_convert.eval_level(getSignal(P_FBM_LKT) * (notePitch)), env_clip_peak);
  params[FBM_LVL] = tmp_lvl * tmp_pan;
}

/* Poly Post Processing - audio parameters */
void paramengine::postProcessPoly_audio(ParameterStorage& params, const uint32_t _voiceId)
{
  /* automatic mono to poly distribution */
  for(auto &it : m_parameters.getPostIds(PARAM_SPREAD, PARAM_AUDIO, PARAM_MONO))
  {
      auto p = getHead(it).m_postId;
      params[p] = getSignal(it);
  }
  /* automatic poly to poly copy - each voice */
  for(auto &it : m_parameters.getPostIds(PARAM_SINGLE, PARAM_AUDIO, PARAM_POLY))
  {
      auto p = getHead(it).m_postId;
      params[p] = getSignal(it, _voiceId);
  }
  /* "NEW" ENVELOPES: */
  //m_new_envelopes.tickMono();
  //}
  /* "NEW" ENVELOPES: */
  /* poly envelope ticking */
  m_new_envelopes.tickPoly(_voiceId);
  /* poly envelope distribution */
  params[ENV_A_MAG] = m_new_envelopes.m_env_a.m_body[_voiceId].m_signal_magnitude
      * getSignal(P_EA_GAIN);  // Envelope A Magnitude post Gain
  params[ENV_A_TMB]
      = m_new_envelopes.m_env_a.m_body[_voiceId].m_signal_timbre * getSignal(P_EA_GAIN);  // Envelope A Timbre post Gain
  params[ENV_B_MAG] = m_new_envelopes.m_env_b.m_body[_voiceId].m_signal_magnitude
      * getSignal(P_EB_GAIN);  // Envelope B Magnitude post Gain
  params[ENV_B_TMB]
      = m_new_envelopes.m_env_b.m_body[_voiceId].m_signal_timbre * getSignal(P_EB_GAIN);  // Envelope B Timbre post Gain
  params[ENV_C_CLIP] = m_new_envelopes.m_env_c.m_body[_voiceId].m_signal_magnitude;       // Envelope C
  params[ENV_G_SIG] = m_new_envelopes.m_env_g.m_body[_voiceId].m_signal_magnitude;        // Gate
  /* reconstruct unclipped envelope c signal by factor */
  params[ENV_C_UNCL] = params[ENV_C_CLIP] * m_env_c_clipFactor[_voiceId];
  /* Oscillator parameter post processing */
  float tmp_amt, tmp_env;
  /* Oscillator A */
  /* - Oscillator A - PM Self */
  tmp_amt = getSignal(P_OA_PMS);
  tmp_env = getSignal(P_OA_PMSEA);
  params[OSC_A_PMSEA]
      = NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_A_TMB], tmp_env) * tmp_amt;  // Osc A PM Self (Env A)
  /* - Oscillator A - PM B */
  tmp_amt = getSignal(P_OA_PMB);
  tmp_env = getSignal(P_OA_PMBEB);
  params[OSC_A_PMBEB]
      = NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_B_TMB], tmp_env) * tmp_amt;  // Osc A PM B (Env B)
  /* - Oscillator A - PM FB */
  tmp_amt = getSignal(P_OA_PMF);
  tmp_env = getSignal(P_OA_PMFEC);
  params[OSC_A_PMFEC]
      = NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_C_CLIP], tmp_env) * tmp_amt;  // Osc A PM FB (Env C)
  /* Shaper A */
  /* - Shaper A Drive (Envelope A) */
  tmp_amt = getSignal(P_SA_DRV);
  tmp_env = getSignal(P_SA_DEA);
  params[SHP_A_DRVEA] = (NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_A_TMB], tmp_env) * tmp_amt) + 0.18f;
  /* - Shaper A Feedback Mix (Envelope C) */
  tmp_env = getSignal(P_SA_FBEC);
  params[SHP_A_FBEC] = NlToolbox::Crossfades::unipolarCrossFade(params[ENV_G_SIG], params[ENV_C_CLIP], tmp_env);
  /* Oscillator B */
  /* - Oscillator B - PM Self */
  tmp_amt = getSignal(P_OB_PMS);
  tmp_env = getSignal(P_OB_PMSEB);
  params[OSC_B_PMSEB]
      = NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_B_TMB], tmp_env) * tmp_amt;  // Osc B PM Self (Env B)
  /* - Oscillator B - PM A */
  tmp_amt = getSignal(P_OB_PMA);
  tmp_env = getSignal(P_OB_PMAEA);
  params[OSC_B_PMAEA]
      = NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_A_TMB], tmp_env) * tmp_amt;  // Osc B PM A (Env A)
  /* - Oscillator B - PM FB */
  tmp_amt = getSignal(P_OB_PMF);
  tmp_env = getSignal(P_OB_PMFEC);
  params[OSC_B_PMFEC]
      = NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_C_CLIP], tmp_env) * tmp_amt;  // Osc B PM FB (Env C)
  /* Shaper B */
  /* - Shaper B Drive (Envelope B) */
  tmp_amt = getSignal(P_SB_DRV);
  tmp_env = getSignal(P_SB_DEB);
  params[SHP_B_DRVEB] = (NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_B_TMB], tmp_env) * tmp_amt) + 0.18f;
  /* - Shaper B Feedback Mix (Envelope C) */
  tmp_env = getSignal(P_SB_FBEC);
  params[SHP_B_FBEC] = NlToolbox::Crossfades::unipolarCrossFade(params[ENV_G_SIG], params[ENV_C_CLIP], tmp_env);
  /* Comb Filter */
  /* - Comb Filter Pitch Envelope C, converted into Frequency Factor */
  tmp_amt = getSignal(P_CMB_PEC);
  params[CMB_FEC] = m_convert.eval_lin_pitch(69.f - (tmp_amt * params[ENV_C_UNCL]));
  /* Decay Time - Gate Signal crossfading */
#if test_comb_decay_gate_mode == 1
  params[CMB_DEC]
      = NlToolbox::Crossfades::unipolarCrossFade(m_comb_decay_times[0], m_comb_decay_times[1], params[ENV_G_SIG]);
#endif
  /* Unison Phase */
#if test_milestone == 150
  params[UN_PHS] = 0.f;
#elif test_milestone == 155
  const uint32_t uVoice = static_cast<uint32_t>(getSignal(P_UN_V));
  const uint32_t uIndex = static_cast<uint32_t>(getSignal(P_KEY_IDX, _voiceId));
  params[UN_PHS] = getSignal(P_UN_PHS) * m_unison_phase[uVoice][uIndex];
#elif test_milestone == 156
  const uint32_t uVoice = static_cast<uint32_t>(getSignal(P_UN_V));
  const uint32_t uIndex = m_unison_index[_voiceId];
  params[UN_PHS] = getSignal(P_UN_PHS) * m_unison_phase[uVoice][uIndex];
#endif
}

/* Poly KEY Post Processing */
void paramengine::postProcessPoly_key(ParameterStorage& params, const uint32_t _voiceId)
{
  /* Pitch Updates */
#if test_milestone == 150
  const float notePitch
      = getSignal(P_KEY_NP, _voiceId) + getSignal(P_MA_T) + m_note_shift[_voiceId];
#elif test_milestone == 155
  const uint32_t uVoice = static_cast<uint32_t>(getSignal(P_UN_V));
  const uint32_t uIndex = static_cast<uint32_t>(getSignal(P_KEY_IDX, _voiceId));
  const float basePitch = getSignal(P_KEY_BP, _voiceId);
  const float notePitch = basePitch + getSignal(P_MA_SH) + (getSignal(P_UN_DET) * m_unison_detune[uVoice][uIndex])
      + m_note_shift[_voiceId];
#elif test_milestone == 156
  const uint32_t uVoice = static_cast<uint32_t>(getSignal(P_UN_V));
  const uint32_t uIndex = m_unison_index[_voiceId];
  const float basePitch = getSignal(P_KEY_BP, _voiceId);
  const float notePitch = basePitch + getSignal(P_MA_SH) + (getSignal(P_UN_DET) * m_unison_detune[uVoice][uIndex])
      + m_note_shift[_voiceId];
#endif
  float keyTracking, unitPitch, envMod, unitSign, unitSpread, unitMod;
  /* Oscillator A */
  /* - Oscillator A Frequency in Hz (Base Pitch, Master Tune, Key Tracking, Osc Pitch, Envelope C) */
  keyTracking = getSignal(P_OA_PKT);
  unitPitch = getSignal(P_OA_P);
  envMod = params[ENV_C_UNCL] * getSignal(P_OA_PEC);
  params[OSC_A_FRQ] = evalNyquist(m_pitch_reference * unitPitch
                                  * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking) + envMod));
  /* - Oscillator A Fluctuation (Envelope C) */
  envMod = getSignal(P_OA_FEC);
  params[OSC_A_FLUEC] = getSignal(P_OA_F) * NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_C_CLIP], envMod);
  /* - Oscillator A Chirp Frequency in Hz */
  params[OSC_A_CHI] = evalNyquist(getSignal(P_OA_CHI) * 440.f);
  /* Oscillator B */
  /* - Oscillator B Frequency in Hz (Base Pitch, Master Tune, Key Tracking, Osc Pitch, Envelope C) */
  keyTracking = getSignal(P_OB_PKT);
  unitPitch = getSignal(P_OB_P);
  envMod = params[ENV_C_UNCL] * getSignal(P_OB_PEC);
  params[OSC_B_FRQ] = evalNyquist(m_pitch_reference * unitPitch
                                  * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking) + envMod));
  /* - Oscillator B Fluctuation (Envelope C) */
  envMod = getSignal(P_OB_FEC);
  params[OSC_B_FLUEC] = getSignal(P_OB_F) * NlToolbox::Crossfades::unipolarCrossFade(1.f, params[ENV_C_CLIP], envMod);
  /* - Oscillator B Chirp Frequency in Hz */
  params[OSC_B_CHI] = evalNyquist(getSignal(P_OB_CHI) * 440.f);
  /* Comb Filter */
  /* - Comb Filter Pitch as Frequency in Hz (Base Pitch, Master Tune, Key Tracking, Comb Pitch) */
  keyTracking = getSignal(P_CMB_PKT);
  unitPitch = getSignal(P_CMB_P);
  // as a tonal component, the reference tone frequency is applied (instead of const 440 Hz)
  params[CMB_FRQ]
      = evalNyquist(m_pitch_reference * unitPitch * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking)));
  /* - Comb Filter Bypass (according to Pitch parameter - without key tracking or reference freq) */
  params[CMB_BYP] = unitPitch > dsp_comb_max_freqFactor
      ? 1.f
      : 0.f;  // check for bypassing comb filter, max_freqFactor corresponds to Pitch of 119.99 ST
  /* - Comb Filter Decay Time (Base Pitch, Master Tune, Gate Env, Dec Time, Key Tracking, Gate Amount) */
  keyTracking = getSignal(P_CMB_DKT);
  envMod = 1.f - ((1.f - params[ENV_G_SIG]) * m_combDecayCurve.applyCurve(getSignal(P_CMB_DG)));
  unitPitch = (-0.5f * notePitch * keyTracking) + (std::abs(getSignal(P_CMB_D)) * envMod);
  unitSign = getSignal(P_CMB_D) < 0 ? -1.f : 1.f;
  params[CMB_DEC] = 0.001f * m_convert.eval_level(unitPitch) * unitSign;
  /* - Comb Filter Allpass Frequency (Base Pitch, Master Tune, Key Tracking, AP Tune, Env C) */
  keyTracking = getSignal(P_CMB_APKT);
  unitPitch = getSignal(P_CMB_APT);
  envMod = params[ENV_C_UNCL] * getSignal(P_CMB_APEC);
  params[CMB_APF] = evalNyquist(440.f * unitPitch
                                * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking)
                                                           + envMod));  // not sure if APF needs Nyquist Clipping?
  //params[CMB_APF] = 440.f * unitPitch * m_convert.eval_lin_pitch(69.f + (basePitch * keyTracking) + envMod);                   // currently APF without Nyquist Clipping
  /* - Comb Filter Lowpass ('Hi Cut') Frequency (Base Pitch, Master Tune, Key Tracking, Hi Cut, Env C) */
  keyTracking = getSignal(P_CMB_LPKT);
  unitPitch = getSignal(P_CMB_LP);
  envMod = params[ENV_C_UNCL] * getSignal(P_CMB_LPEC);
  params[CMB_LPF] = evalNyquist(440.f * unitPitch
                                * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking)
                                                           + envMod));  // not sure if LPF needs Nyquist Clipping?
  //params[CMB_LPF] = 440.f * unitPitch * m_convert.eval_lin_pitch(69.f + (basePitch * keyTracking) + envMod);                   // currently LPF without Nyquist Clipping
  /* State Variable Filter */
  /* - Cutoff Frequencies */
  keyTracking = getSignal(P_SVF_CKT);                  // get Key Tracking
  envMod = params[ENV_C_UNCL] * getSignal(P_SVF_CEC);  // get Envelope C Modulation (amount * envelope_c_signal)
  // as a tonal component, the Reference Tone frequency is applied (instead of const 440 Hz)
  unitPitch = m_pitch_reference * getSignal(P_SVF_CUT);
  unitSpread = getSignal(P_SVF_SPR);  // get the Spread parameter (already scaled to 50%)
  unitMod = getSignal(P_SVF_FM);      // get the FM parameter
  /*   now, calculate the actual filter frequencies and put them in the shared signal array */
  params[SVF_F1_CUT] = evalNyquist(unitPitch
                                   * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking) + envMod
                                                              + unitSpread));  // SVF upper 2PF Cutoff Frequency
  params[SVF_F2_CUT] = evalNyquist(unitPitch
                                   * m_convert.eval_lin_pitch(69.f + (notePitch * keyTracking) + envMod
                                                              - unitSpread));  // SVF lower 2PF Cutoff Frequency
  params[SVF_F1_FM] = params[SVF_F1_CUT] * unitMod;                            // SVF upper 2PF FM Amount (Frequency)
  params[SVF_F2_FM] = params[SVF_F2_CUT] * unitMod;                            // SVF lower 2PF FM Amount (Frequency)
  /* - Resonance */
  keyTracking = getSignal(P_SVF_RKT) * m_svfResFactor;
  envMod = params[ENV_C_CLIP] * getSignal(P_SVF_REC);
  unitPitch = getSignal(P_SVF_RES) + envMod + (notePitch * keyTracking);
  //params[SVF_RES] = m_svfResonanceCurve.applyCurve(std::clamp(unitPitch, 0.f, 1.f));
  float res = 1.f
      - m_svfResonanceCurve.applyCurve(
            std::clamp(unitPitch, 0.f, 1.f));  // NEW resonance handling directly in post processing
  params[SVF_RES] = std::max(res + res, 0.02f);
  /* Output Mixer */
  float tmp_lvl, tmp_pan;
#if test_milestone == 150
  const float poly_pan = getSignal(P_KEY_VP, _voiceId);
#elif test_milestone == 155
  const float poly_pan
      = (getSignal(P_OM_KP) * (basePitch - 6.f)) + (getSignal(P_UN_PAN) * m_unison_pan[uVoice][uIndex]);
#elif test_milestone == 156
  const float poly_pan
      = (getSignal(P_OM_KP) * (basePitch - 6.f)) + (getSignal(P_UN_PAN) * m_unison_pan[uVoice][uIndex]);
#endif
  /* - Branch A */
  tmp_lvl = getSignal(P_OM_AL);
  tmp_pan = std::clamp(getSignal(P_OM_AP) + poly_pan, 0.f, 1.f);
  params[OUT_A_L] = tmp_lvl * (1.f - tmp_pan);
  params[OUT_A_R] = tmp_lvl * tmp_pan;
  /* - Branch B */
  tmp_lvl = getSignal(P_OM_BL);
  tmp_pan = std::clamp(getSignal(P_OM_BP) + poly_pan, 0.f, 1.f);
  params[OUT_B_L] = tmp_lvl * (1.f - tmp_pan);
  params[OUT_B_R] = tmp_lvl * tmp_pan;
  /* - Comb Filter */
  tmp_lvl = getSignal(P_OM_CL);
  tmp_pan = std::clamp(getSignal(P_OM_CP) + poly_pan, 0.f, 1.f);
  params[OUT_CMB_L] = tmp_lvl * (1.f - tmp_pan);
  params[OUT_CMB_R] = tmp_lvl * tmp_pan;
  /* - State Variable Filter */
  tmp_lvl = getSignal(P_OM_SL);
  tmp_pan = std::clamp(getSignal(P_OM_SP) + poly_pan, 0.f, 1.f);
  params[OUT_SVF_L] = tmp_lvl * (1.f - tmp_pan);
  params[OUT_SVF_R] = tmp_lvl * tmp_pan;
  /* - Feedback Mixer */
  tmp_lvl = getSignal(P_FBM_LVL);
  tmp_pan = std::min(m_convert.eval_level(getSignal(P_FBM_LKT) * (notePitch)), env_clip_peak);
  params[FBM_LVL] = tmp_lvl * tmp_pan;
  /*   - determine Highpass Filter Frequency */
  params[FBM_HPF] = evalNyquist(m_convert.eval_lin_pitch(12.f + notePitch) * 440.f);
}

/* Mono Post Processing - slow parameters */
void paramengine::postProcessMono_slow(ParameterStorage& params)
{
  /* automatic mono to mono copy (always voice zero) */
  for(auto &it : m_parameters.getPostIds(PARAM_SINGLE, PARAM_SLOW, PARAM_MONO))
  {
      auto p = getHead(it).m_postId;
      params[p] = getSignal(it);
  }
  /* Effect Parameter Post Processing */
  float tmp_Gap, tmp_Center, tmp_Stereo, tmp_Rate;
  const float tmp_SR = static_cast<float>(m_samplerate);
  /* - Flanger */
#if test_flanger_phs == 0
  /*   - LFO/Envelope Rate, Phase */
  m_flangerLFO.m_phaseOffset = m_flaNormPhase * getSignal(P_FLA_PHS);
#endif
  tmp_Rate = getSignal(P_FLA_RTE);
  m_flangerLFO.m_increment = m_reciprocal_samplerate * tmp_Rate;
  tmp_Rate *= m_flangerRateToDecay;
  m_new_envelopes.m_env_f.setSegmentDx(0, 2, tmp_Rate);
  /*   - Stereo Time */
  tmp_Center = getSignal(P_FLA_TIME) * dsp_samples_to_ms * tmp_SR;  // time (in ms) is handled in samples
  tmp_Stereo = getSignal(P_FLA_STE) * 0.01f;
  params[FLA_TL] = tmp_Center * (1.f + tmp_Stereo);
  params[FLA_TR] = tmp_Center * (1.f - tmp_Stereo);
  /*   - Hi Cut Frequency */
  params[FLA_LPF] = evalNyquist(getSignal(P_FLA_LPF) * 440.f);
  /* - Cabinet */
  /*   - Hi Cut Frequency in Hz (Hi Cut == Lowpass) */
  params[CAB_LPF] = evalNyquist(getSignal(P_CAB_LPF) * 440.f);
  /*   - Lo Cut Frequency in Hz (Lo Cut == Highpass) */
  params[CAB_HPF] = evalNyquist(getSignal(P_CAB_HPF) * 440.f);  // nyquist clipping not necessary...
  /*   - Tilt to Shelving EQs */
  params[CAB_TILT] = getSignal(P_CAB_TILT);
  /* - Gap Filter */
  tmp_Gap = (getSignal(P_GAP_MIX) < 0.f ? -1.f : 1.f) * getSignal(P_GAP_GAP);
  tmp_Center = getSignal(P_GAP_CNT);
  tmp_Stereo = getSignal(P_GAP_STE);
  /*   - Left LP Frequency in Hz */
  params[GAP_LFL] = evalNyquist(m_convert.eval_lin_pitch(tmp_Center - tmp_Gap - tmp_Stereo)
                                * 440.f);  // nyquist clipping not necessary...
  /*   - Left HP Frequency in Hz */
  params[GAP_HFL] = evalNyquist(m_convert.eval_lin_pitch(tmp_Center + tmp_Gap - tmp_Stereo) * 440.f);
  /*   - Right LP Frequency in Hz */
  params[GAP_LFR] = evalNyquist(m_convert.eval_lin_pitch(tmp_Center - tmp_Gap + tmp_Stereo)
                                * 440.f);  // nyquist clipping not necessary...
  /*   - Right HP Frequency in Hz */
  params[GAP_HFR] = evalNyquist(m_convert.eval_lin_pitch(tmp_Center + tmp_Gap + tmp_Stereo) * 440.f);
  /* - Echo */
  /*   - Time and Stereo */
  tmp_Center = getSignal(P_DLY_TIME) * tmp_SR;  // time is handled in samples
  tmp_Stereo = getSignal(P_DLY_STE) * m_dlyNormStereo;
  params[DLY_TL] = tmp_Center * (1.f + tmp_Stereo);
  params[DLY_TR] = tmp_Center * (1.f - tmp_Stereo);
  /*   - High Cut Frequency */
  params[DLY_LPF] = evalNyquist(getSignal(P_DLY_LPF) * 440.f);
  /* - Reverb (if slow rendering is enabled - see pe_defines_config.h) */
#if test_reverbParams == 1
  float tmp_val, tmp_fb, tmp_dry, tmp_wet;
  /*   - Size to Size, Feedback, Balance */
  tmp_val = getSignal(P_REV_SIZE);
  tmp_val *= 2.f - std::abs(tmp_val);
  params[REV_SIZE] = tmp_val;
  tmp_fb = tmp_val * (0.6f + (0.4f * std::abs(tmp_val)));
  params[REV_FEED] = 4.32f - (3.32f * tmp_fb);
  tmp_fb = tmp_val * (1.3f - (0.3f * std::abs(tmp_val)));
  params[REV_BAL] = 0.9f * tmp_fb;
  /*   - Pre Delay */
  params[REV_PRE] = getSignal(P_REV_PRE) * 200.f * m_millisecond;
  /*   - Color to Filter Frequencies (HPF, LPF) */
  tmp_val = getSignal(P_REV_COL);
  params[REV_LPF] = evalNyquist(m_convert.eval_lin_pitch(m_revColorCurve1.applyCurve(tmp_val)) * 440.f);
  params[REV_HPF] = evalNyquist(m_convert.eval_lin_pitch(m_revColorCurve2.applyCurve(tmp_val)) * 440.f);
  /*   - Mix to Dry, Wet */
  tmp_val = getSignal(P_REV_MIX);
  tmp_dry = 1.f - tmp_val;
  tmp_dry = (2.f - tmp_dry) * tmp_dry;
  params[REV_DRY] = tmp_dry;
  tmp_wet = tmp_val;
  tmp_wet = (2.f - tmp_wet) * tmp_wet;
  params[REV_WET] = tmp_wet;
#endif
}

/* Mono Post Processing - fast parameters */
void paramengine::postProcessMono_fast(ParameterStorage& params)
{
  /* automatic mono to mono copy (always voice zero) */
  for(auto &it : m_parameters.getPostIds(PARAM_SINGLE, PARAM_FAST, PARAM_MONO))
  {
      auto p = getHead(it).m_postId;
      params[p] = getSignal(it);
  }
  /* Explicit Post Processing */
  /* provide temporary variables */
  float tmp_val, tmp_dry, tmp_wet, tmp_hi_par, tmp_lo_par, tmp_hi_ser, tmp_lo_ser, tmp_fb;
  /* Effect Parameter Post Processing */
  /* - Flanger */
#if test_flanger_phs == 1
  /*   - LFO/Envelope Rate, Phase */
  m_flangerLFO.m_phaseOffset = m_flaNormPhase * getSignal(P_FLA_PHS);
#endif
  /*   - Feedback and Cross Feedback */
  tmp_fb = m_flaFeedbackCurve.applyCurve(getSignal(P_FLA_FB));
  tmp_val = getSignal(P_FLA_CFB);
  params[FLA_FB_LOC] = tmp_fb * (1.f - std::abs(tmp_val));
  params[FLA_FB_CR] = tmp_fb * tmp_val;
  /*   - Dry and Wet Amounts */
  tmp_val = getSignal(P_FLA_MIX);
  params[FLA_DRY] = 1.f - std::abs(tmp_val);
  params[FLA_WET] = tmp_val;
  /*   - Allpass Frequencies */
  tmp_val = getSignal(P_FLA_APM);
  tmp_dry = getSignal(P_FLA_APT);
  params[FLA_APF_L] = evalNyquist(m_convert.eval_lin_pitch((params[FLA_LFO_L] * tmp_val) + tmp_dry) * 440.f);
  params[FLA_APF_R] = evalNyquist(m_convert.eval_lin_pitch((params[FLA_LFO_R] * tmp_val) + tmp_dry) * 440.f);
  /* - Cabinet */
  /*   - Tilt to Saturation Levels (pre, post Shaper) */
  tmp_val = std::max(m_cabTiltFloor, m_convert.eval_level(0.5f * getSignal(P_CAB_TILT)));
  params[CAB_PRESAT] = 0.1588f / tmp_val;
  params[CAB_SAT] = tmp_val;
  /*   - Cab Level and Dry/Wet Mix Levels */
  params[CAB_DRY] = 1.f - getSignal(P_CAB_MIX);
  params[CAB_WET] = getSignal(P_CAB_LVL) * getSignal(P_CAB_MIX);
  /* - Gap Filter */
  tmp_val = std::abs(getSignal(P_GAP_MIX));
  tmp_wet = NlToolbox::Math::sin(NlToolbox::Constants::halfpi * tmp_val);
  tmp_dry = NlToolbox::Math::sin(NlToolbox::Constants::halfpi * (1.f - tmp_val));
  tmp_val = getSignal(P_GAP_BAL);
  tmp_hi_par = 0.5f + (0.5f * tmp_val);
  tmp_lo_par = 1.f - tmp_hi_par;
  tmp_hi_par = NlToolbox::Math::sin(NlToolbox::Constants::halfpi * tmp_hi_par) * NlToolbox::Constants::sqrt_two;
  tmp_lo_par = NlToolbox::Math::sin(NlToolbox::Constants::halfpi * tmp_lo_par) * NlToolbox::Constants::sqrt_two;
  tmp_val *= tmp_val;
  tmp_hi_ser = getSignal(P_GAP_BAL) > 0.f ? tmp_val : 0.f;
  tmp_lo_ser = getSignal(P_GAP_BAL) > 0.f ? 0.f : tmp_val;
  if(getSignal(P_GAP_MIX) > 0.f)
  {
    /* parallel mode */
    /* - HP to LP input signal */
    params[GAP_HPLP] = 0.f;
    /* - LP input signal */
    params[GAP_INLP] = 1.f;
    /* - HP output signal */
    params[GAP_HPOUT] = tmp_wet * tmp_hi_par;
    /* - LP output signal */
    params[GAP_LPOUT] = tmp_wet * tmp_lo_par;
    /* - Main output signal */
    params[GAP_INOUT] = tmp_dry;
  }
  else
  {
    /* serial mode */
    tmp_val = tmp_wet * tmp_hi_ser;
    /* - HP to LP input signal */
    params[GAP_HPLP] = 1.f - tmp_lo_ser;
    /* - LP input signal */
    params[GAP_INLP] = tmp_lo_ser;
    /* - HP output signal */
    params[GAP_HPOUT] = params[GAP_HPLP] * tmp_val;
    /* - LP output signal */
    params[GAP_LPOUT] = tmp_wet - tmp_val;
    /* - Main output signal */
    params[GAP_INOUT] = (tmp_val * tmp_lo_ser) + tmp_dry;
  }
  /* - Echo */
  /*   - Feedback and Cross Feedback */
  tmp_fb = getSignal(P_DLY_FB);
  tmp_val = getSignal(P_DLY_CFB);
  params[DLY_FB_LOC] = tmp_fb * (1.f - tmp_val);
  params[DLY_FB_CR] = tmp_fb * tmp_val;
  /*   - Dry and Wet Mix Amounts */
  tmp_val = getSignal(P_DLY_MIX);
  params[DLY_WET] = (2.f * tmp_val) - (tmp_val * tmp_val);
  tmp_val = 1.f - tmp_val;
  params[DLY_DRY] = (2.f * tmp_val) - (tmp_val * tmp_val);
  /* - Reverb (if fast rendering is enabled - see pe_defines_config.h) */
#if test_reverbParams == 0
  /*   - Size to Size, Feedback, Balance */
  tmp_val = getSignal(P_REV_SIZE);
  tmp_val *= 2.f - std::abs(tmp_val);
  params[REV_SIZE] = tmp_val;
  tmp_fb = tmp_val * (0.6f + (0.4f * std::abs(tmp_val)));
  params[REV_FEED] = 4.32f - (3.32f * tmp_fb);
  tmp_fb = tmp_val * (1.3f - (0.3f * std::abs(tmp_val)));
  params[REV_BAL] = 0.9f * tmp_fb;
  /*   - Pre Delay */
  params[REV_PRE] = getSignal(P_REV_PRE) * 200.f * m_millisecond;
  /*   - Color to Filter Frequencies (HPF, LPF) */
  tmp_val = getSignal(P_REV_COL);
  params[REV_LPF] = evalNyquist(m_convert.eval_lin_pitch(m_revColorCurve1.applyCurve(tmp_val)) * 440.f);
  params[REV_HPF] = evalNyquist(m_convert.eval_lin_pitch(m_revColorCurve2.applyCurve(tmp_val)) * 440.f);
  /*   - Mix to Dry, Wet */
  tmp_val = getSignal(P_REV_MIX);
  tmp_dry = 1.f - tmp_val;
  tmp_dry = (2.f - tmp_dry) * tmp_dry;
  params[REV_DRY] = tmp_dry;
  tmp_wet = tmp_val;
  tmp_wet = (2.f - tmp_wet) * tmp_wet;
  params[REV_WET] = tmp_wet;
#endif
}

/* Mono Post Processing - audio parameters */
void paramengine::postProcessMono_audio(ParameterStorage& params)
{
  /* automatic mono to mono copy (always voice zero) */
  for(auto &it : m_parameters.getPostIds(PARAM_SINGLE, PARAM_AUDIO, PARAM_MONO))
  {
      auto p = getHead(it).m_postId;
      params[p] = getSignal(it);
  }
  /* mono envelope rendering */
  float tmp_env;
  /* "NEW" ENVELOPES: */
  m_new_envelopes.tickMono();
  tmp_env = (m_new_envelopes.m_env_f.m_body[0].m_signal_magnitude * 2.f) - 1.f;
  /* - Flanger */
#if test_flanger_phs == 2
  /*   - LFO/Envelope Rate, Phase */
  m_flangerLFO.m_phaseOffset = m_flaNormPhase * getSignal(P_FLA_PHS);
#endif
  /*   - render LFO, crossfade flanger envelope and pass left and right signals to array */
  m_flangerLFO.tick();
  float tmp_wet = getSignal(P_FLA_ENV);
  float tmp_dry = 1.f - tmp_wet;
  params[FLA_LFO_L] = (tmp_dry * m_flangerLFO.m_left) + (tmp_wet * tmp_env);
  params[FLA_LFO_R] = (tmp_dry * m_flangerLFO.m_right) + (tmp_wet * tmp_env);
}

void paramengine::testLevelVelocity()
{
  //
  float LV[5];
  LV[0] = 0.f;
  LV[1] = 15.f;
  LV[2] = 30.f;
  LV[3] = 45.f;
  LV[4] = 60.f;
  //
  float KV[5];
  KV[0] = 0.f;
  KV[1] = 0.25f;
  KV[2] = 0.5f;
  KV[3] = 0.75f;
  KV[4] = 1.f;
  //
  uint32_t LVI, KVI;
  float peak;
  //
  Log::info("Test Level Velocities:");
  Log::info("\t0\t15\t30\t45\t60\t(dB Level Velocity)");
  for(KVI = 0; KVI < 5; KVI++)
  {
    Log::info<Log::LogMode::Plain>(100 * KV[KVI], "\t");
    for(LVI = 0; LVI < 5; LVI++)
    {
      peak = m_convert.eval_level((1.f - KV[KVI]) * -LV[LVI]);
      Log::info<Log::LogMode::Plain>(peak, "\t");
    }
    Log::info();
  }
  Log::info("(% Key Velocity)");
}

void paramengine::testRounding()
{
  Log::info("Testing Round Mechanism");
  Log::info("\tidx\tfloat\tint\tround\tint+\tint-\tR5?");
  float f;
  for(uint32_t i = 0; i < 65; i++)
  {
    f = roundingValues[i];
    Log::info<Log::LogMode::Plain>("\t", i, "\t", f, "\t", static_cast<int>(f), "\t");
    Log::info<Log::LogMode::Plain>(static_cast<int>(std::round(f - 0.5f)), "\t", static_cast<int>(f + 0.5f), "\t");
    Log::info<Log::LogMode::AppendNewLine>(static_cast<int>(f - 0.5f));
    // reaktor algorithm???
  }
  Log::info();
}
