#pragma once

/******************************************************************************/
/** @file       ae_poly_section.h
    @date
    @version    1.7-3
    @author     M. Seeber
    @brief      new container for all polyphonic parameters and dsp
    @todo
*******************************************************************************/

#include "key_event.h"
#include "smoother_handle.h"
#include "unison_spread_table.h"
#include "pe_exponentiator.h"
#include "ae_info.h"
#include "ae_envelopes.h"
#include "ae_poly_soundgen.h"
#include "ae_poly_comb.h"
#include "ae_poly_svf.h"
#include "ae_poly_fb_mix.h"
#include "ae_poly_out_mix.h"
#include "nltoolbox.h"

class PolySection
{
 public:
  PolySignals m_signals;
  float *m_reference;
  float m_send_self_l = 0.0f, m_send_self_r = 0.0f, m_send_other_l = 0.0f, m_send_other_r = 0.0f, m_millisecond = 0.0f;
  uint32_t m_uVoice = 0, m_key_active = 0;
  int32_t m_fadeStart = 0, m_fadeEnd = 0, m_fadeIncrement = 0;
  PolySection();
  void init(GlobalSignals *_globalsignals, exponentiator *_convert, Engine::Handle::Time_Handle *_time,
            LayerSignalCollection *_z_self, float *_reference, const float _ms, const float _gateRelease,
            const float _samplerate, const uint32_t _upsampleFactor);
  void add_copy_sync_id(const uint32_t _smootherId, const uint32_t _signalId);
  void add_copy_audio_id(const uint32_t _smootherId, const uint32_t _signalId);
  void add_copy_fast_id(const uint32_t _smootherId, const uint32_t _signalId);
  void add_copy_slow_id(const uint32_t _smootherId, const uint32_t _signalId);
  void start_sync(const uint32_t _id, const float _dest);
  void start_audio(const uint32_t _id, const float _dx, const float _dest);
  void start_fast(const uint32_t _id, const float _dx, const float _dest);
  void start_slow(const uint32_t _id, const float _dx, const float _dest);
  void render_audio(const float _mute);
  void render_feedback(const LayerSignalCollection &_z_other);
  void render_fast();
  void render_slow();
  bool keyDown(PolyKeyEvent *_event);
  void keyUp(PolyKeyEvent *_event);
  void resetEnvelopes();
  void flushDSP();
  void resetDSP();
  float getVoiceGroupVolume();
  void evalVoiceFade(const float _from, const float _range);
  void evalVoiceFadeInterpolated(const float _from, const float _range);
  void resetVoiceFade();

 private:
  float m_key_levels[C15::Config::virtual_key_count] = {};
  SmootherHandle<C15::Smoothers::Poly_Sync, C15::Smoothers::Poly_Audio, C15::Smoothers::Poly_Fast,
                 C15::Smoothers::Poly_Slow>
      m_smoothers;
  GlobalSignals *m_globalsignals;
  exponentiator *m_convert;
  Engine::Handle::Time_Handle *m_time;
  LayerSignalCollection *m_z_self;
  UnisonSpreadTable<C15::Config::total_polyphony> m_spread;
  Engine::Envelopes::SplitEnvelope<C15::Config::local_polyphony> m_env_a, m_env_b;
  Engine::Envelopes::RetriggerEnvelope<C15::Config::local_polyphony> m_env_c;
  Engine::Envelopes::GateEnvelope<C15::Config::local_polyphony> m_env_g;
  Engine::PolySoundGenerator m_soundgenerator;
  Engine::PolyCombFilter m_combfilter;
  Engine::PolyStateVariableFilter m_svfilter;
  Engine::PolyFeedbackMixer m_feedbackmixer;
  Engine::PolyOutputMixer m_outputmixer;
  NlToolbox::Curves::Shaper_1_BP m_comb_decayCurve, m_svf_LBH1Curve, m_svf_LBH2Curve;
  NlToolbox::Curves::Shaper_2_BP m_svf_resCurve;
  ProtoSmoother<1> m_mono_glide;
  PolyValue m_comb_decay_times[2] = {}, m_voice_level = {};
#if POTENTIAL_IMPROVEMENT_GAIN_CURVE == __POTENTIAL_IMPROVEMENT_ENABLED__
  std::vector<float> m_gain_curve_buffer;
  PolyInt m_gain_curve_index = {};
  PolyValue m_voice_level_old = {};
  uint32_t m_gain_curve_size = {}, m_gain_curve_size_m1 = {};
#endif
  const float m_svf_resFactor = 1.0f / 60.0f;
  float m_note_pitch[C15::Config::local_polyphony] = {}, m_base_pitch[C15::Config::local_polyphony] = {},
        m_key_tune[C15::Config::local_polyphony] = {}, m_last_key_tune[C15::Config::local_polyphony] = {},
        m_samplerate = 0.0f, m_nyquist = 0.0f;
  uint32_t m_key_position[C15::Config::local_polyphony] = {}, m_unison_index[C15::Config::local_polyphony] = {};
  const uint32_t m_voices = C15::Config::local_polyphony;
  float evalNyquist(const float _value);
  float evalScale(const uint32_t _voiceId);
  void postProcess_poly_audio(const uint32_t _voiceId, const float _mute);
  void postProcess_poly_fast(const uint32_t _voiceId);
  void postProcess_mono_fast();
  void postProcess_poly_slow(const uint32_t _voiceId);
  void postProcess_mono_slow();
  void postProcess_poly_pitch(const uint32_t _voiceId, const float _envC);
  void postProcess_poly_key(const uint32_t _voiceId);
  void startEnvelopes(const uint32_t _voiceId, const float _pitch, const float _vel);
  void stopEnvelopes(const uint32_t _voiceId, const float _pitch, const float _vel);
  void updateEnvLevels(const uint32_t _voiceId);
  void updateEnvTimes(const uint32_t _voiceId);
  void updateNotePitch(const uint32_t _voiceId);
  void setSlowFilterCoefs(const uint32_t _voiceId);
  void testVoiceFadeTable();
};
