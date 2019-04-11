/******************************************************************************/
/** @file		paramengine.h
    @date		2018-03-08
    @version    0.1
    @author		Matthias Seeber & Anton Schmied[2018-01-23]
    @brief		an implementation of the Parameter Renderer
                and TDC Interpreter

    @todo
*******************************************************************************/

#pragma once

#include <stdint.h>
#include "pe_defines_params.h"
#include "pe_exponentiator.h"
#include "pe_defines_config.h"
#include "pe_env_engine2.h"
#include "pe_key_event.h"
#include "pe_lfo_engine.h"
#include "pe_utilities.h"
#include "pe_defines_labels.h"
#include "dsp_defines_signallabels.h"
#include "nltoolbox.h"

class ParameterStorage;

/* */
struct param_head
{
  /* */
  uint32_t m_id;
  uint32_t m_index;
  uint32_t m_size;
  uint32_t m_clockType;
  uint32_t m_polyType;
  uint32_t m_scaleId;
  uint32_t m_postId;
  float m_normalize;
  float m_scaleArg;
};

/* */
struct param_body
{
  /* */
  uint32_t m_state = 0;
  uint32_t m_preload = 0;
  float m_signal = 0.f;
  float m_dx[2] = { 0.f, 0.f };
  float m_x = 0.f;
  float m_start = 0.f;
  float m_diff = 0.f;
  float m_dest = 0.f;
};

/* */
struct param_utility
{
  /* */
  uint32_t m_scaleId;
  float m_normalize;
  float m_scaleArg;
};

struct Parameters
{

  inline param_head &getHead(uint32_t id)
  {
    return m_head[id];
  }

  inline param_body &getBody(uint32_t id)
  {
    return m_body[id];
  }

  inline float getSignal(uint32_t paramId) const
  {
    return m_body[m_head[paramId].m_index].m_signal;
  }

  inline float getSignal(uint32_t paramId, uint32_t voice) const
  {
    return m_body[m_head[paramId].m_index + voice].m_signal;
  }

  inline void addClockId(uint32_t _clockType, uint32_t _polyType, uint32_t _id)
  {
      m_clockIds.add(_clockType, _polyType, _id);
  }

  inline const std::vector<uint32_t> &getClockIds(uint32_t _clockType, uint32_t _polyType) const
  {
      return m_clockIds.get(_clockType, _polyType);
  }

  inline void addPostId(uint32_t _spreadType, uint32_t _clockType, uint32_t _polyType, uint32_t _id)
  {
      m_postIds.add(_spreadType, _clockType, _polyType, _id);
  }

  inline const std::vector<uint32_t> &getPostIds(uint32_t _spreadType, uint32_t _clockType, uint32_t _polyType) const
  {
      return m_postIds.get(_spreadType, _clockType, _polyType);
  }

private:
  param_head m_head[sig_number_of_params];
  param_body m_body[sig_number_of_param_items];
  new_clock_id_list m_clockIds;
  new_dual_clock_id_list m_postIds;
};

/* improving code readability */
enum PARAM_CLOCK_TYPES
{
    PARAM_SYNC = 0,
    PARAM_AUDIO = 1,
    PARAM_FAST = 2,
    PARAM_SLOW = 3
};

enum PARAM_POLY_TYPES
{
    PARAM_MONO = 0,
    PARAM_POLY = 1
};

enum PARAM_SPREAD_TYPES
{
    PARAM_SINGLE = 0,
    PARAM_SPREAD = 1
};

/* */
struct paramengine
{

  Parameters m_parameters;

  inline param_head &getHead(uint32_t id)
  {
    return m_parameters.getHead(id);
  }

  inline param_body &getBody(uint32_t id)
  {
    return m_parameters.getBody(id);
  }

  inline float getSignal(uint32_t paramId) const
  {
    return m_parameters.getSignal(paramId);
  }

  inline float getSignal(uint32_t paramId, uint32_t voice) const
  {
    return m_parameters.getSignal(paramId, voice);
  }

  /* local variables */
  uint32_t m_samplerate;
  uint32_t m_preload = 0;
  /* pitch reference (A3 = 440.0 Hz) */
  float m_pitch_reference = dsp_init_pitch_reference;
  /* samplerate dependant parameters */
  float m_nyquist_frequency;
  float m_millisecond;
  float m_reciprocal_samplerate;
  /* clock and polyphony definitions */
  float m_timeFactors[dsp_clock_types];
  uint32_t m_routePolyphony[dsp_poly_types] = { 1, dsp_number_of_voices };
  /* access to envelope parameters */
  const uint32_t m_envIds[sig_number_of_env_events] = { P_EA, P_EB, P_EC };
  /* local data structures */
  exponentiator m_convert;
  param_utility m_utilities[sig_number_of_utilities];
  float m_env_c_clipFactor[dsp_number_of_voices];
  env_engine2 m_new_envelopes;
  poly_key_event m_event;
#if test_comb_decay_gate_mode == 1
  float m_comb_decay_times[2];
#endif
  NlToolbox::Curves::Shaper_1_BP m_combDecayCurve;
  NlToolbox::Curves::Shaper_1_BP m_svfLBH1Curve;
  NlToolbox::Curves::Shaper_1_BP m_svfLBH2Curve;
  const float m_svfResFactor = 1.f / 60.f;
  const float m_cabTiltFloor = 2.e-20f;
  NlToolbox::Curves::Shaper_2_BP m_svfResonanceCurve;
  const float m_dlyNormStereo = 1.f / 99.f;
  NlToolbox::Curves::Shaper_2_BP m_flaFeedbackCurve;
  const float m_flaNormPhase = 1.f / 360.f;
  stereo_lfo m_flangerLFO;
  float m_flangerRateToDecay = 0.55f;
  //float m_flangerEnv[10];
  NlToolbox::Curves::Shaper_1_BP m_revColorCurve1;
  NlToolbox::Curves::Shaper_1_BP m_revColorCurve2;
  float m_unison_detune[dsp_number_of_voices][dsp_number_of_voices],
      m_unison_phase[dsp_number_of_voices][dsp_number_of_voices],
      m_unison_pan[dsp_number_of_voices][dsp_number_of_voices], m_note_shift[dsp_number_of_voices];
#if test_milestone == 156
  uint32_t m_unison_index[dsp_number_of_voices] = {};
#endif
  /* proper init */
  void init(uint32_t _sampleRate, uint32_t _voices);
  /* helper */
  float evalNyquist(float _freq);
  /* TCD mechanism */
  float scale(const uint32_t _scaleId, const float _scaleArg, float _value);     // provided tcd scale functions
  void setDx(const uint32_t _voiceId, const uint32_t _paramId, float _value);    // param dx update
  void setDest(const uint32_t _voiceId, const uint32_t _paramId, float _value);  // param dest update
  void applyPreloaded(const uint32_t _voiceId, const uint32_t _paramId);         // param apply preloaded
  void applyDest(const uint32_t _index);                                         // param apply dest (non-sync types)
  void applySync(const uint32_t _index);                                         // param apply dest (sync types)
  /* rendering */
  void tickItem(const uint32_t _index);  // parameter rendering
  /* key events */
  void keyDown(const uint32_t _voiceId, float _velocity);  // key events: key down (note on) mechanism
  void keyUp(const uint32_t _voiceId, float _velocity);    // key events: key up (note off) mechanism
  void keyApply(const uint32_t _voiceId);                  // key events: apply key event
  void keyApplyMono();                                     // key events: apply mono event
  /* NEW envelopes updates */
  void newEnvUpdateStart(const uint32_t _voiceId, const float _pitch, const float _velocity);
  void newEnvUpdateStop(const uint32_t _voiceId, const float _pitch, const float _velocity);
  void newEnvUpdateTimes(const uint32_t _voiceId);
  void newEnvUpdateLevels(const uint32_t _voiceId);
  /* simplified polyphonic post processing approach (one function per clock) */
  void postProcessPoly_slow(
      ParameterStorage &params,
      const uint32_t _voiceId);  // poly slow post processing (distribution, copy, env c event signal!)
  void postProcessPoly_fast(ParameterStorage &params,
                            const uint32_t _voiceId);  // poly fast post processing (distribution, copy)
  void postProcessPoly_audio(
      ParameterStorage &params,
      const uint32_t _voiceId);  // poly audio post processing (distribution, copy, envelopes, param combination)
  void postProcessPoly_key(
      ParameterStorage &params,
      const uint32_t _voiceId);  // NEW poly key post processing (only required signals should be updated)
  void postProcessMono_slow(ParameterStorage &params);   // mono slow post processing
  void postProcessMono_fast(ParameterStorage &params);   // mono fast post processing
  void postProcessMono_audio(ParameterStorage &params);  // mono audio post processing
  /* temporary tests */
  void testLevelVelocity();
  const float roundingValues[65] = {
    // temporary array for testing purposes
    -8.f,   -7.75f, -7.5f,  -7.25f, -7.f,   -6.75f, -6.5f,  -6.25f, -6.f,   -5.75f, -5.5f,  -5.25f, -5.f,
    -4.75f, -4.5f,  -4.25f, -4.f,   -3.75f, -3.5f,  -3.25f, -3.f,   -2.75f, -2.5f,  -2.25f, -2.f,   -1.75f,
    -1.5f,  -1.25f, -1.f,   -0.75f, -0.5f,  -0.25f, 0.f,    0.25f,  0.5f,   0.75f,  1.f,    1.25f,  1.5f,
    1.75f,  2.f,    2.25f,  2.5f,   2.75f,  3.f,    3.25f,  3.5f,   3.75f,  4.f,    4.25f,  4.5f,   4.75f,
    5.f,    5.25f,  5.5f,   5.75f,  6.f,    6.25f,  6.5f,   6.75f,  7.f,    7.25f,  7.5f,   7.75f,  8.f,
  };
  void testRounding();
};
