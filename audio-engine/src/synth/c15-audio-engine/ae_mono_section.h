#pragma once

/******************************************************************************/
/** @file       ae_mono_section.h
    @date
    @version    1.7-3
    @author     M. Seeber
    @brief      new container for all monophonic parameters and dsp
    @todo
*******************************************************************************/

#include "key_event.h"
#include "smoother_handle.h"
#include "ae_info.h"
#include "ae_envelopes.h"
#include "ae_mono_flanger.h"
#include "ae_mono_cabinet.h"
#include "ae_mono_gapfilter.h"
#include "ae_mono_echo.h"
#include "ae_mono_reverb.h"
#include "pe_exponentiator.h"
#include "pe_lfo_engine.h"

class MonoSection
{
 public:
  MonoSignals m_signals;
  float m_out_l = 0.0f, m_out_r = 0.0f, m_dry = 0.0f, m_wet = 0.0f;
  MonoSection();
  void init(exponentiator *_convert, LayerSignalCollection *_z_self, const float _ms, const float _samplerate);
  void add_copy_sync_id(const uint32_t _smootherId, const uint32_t _signalId);
  void add_copy_audio_id(const uint32_t _smootherId, const uint32_t _signalId);
  void add_copy_fast_id(const uint32_t _smootherId, const uint32_t _signalId);
  void add_copy_slow_id(const uint32_t _smootherId, const uint32_t _signalId);
  void start_sync(const uint32_t _id, const float _dest);
  void start_audio(const uint32_t _id, const float _dx, const float _dest);
  void start_fast(const uint32_t _id, const float _dx, const float _dest);
  void start_slow(const uint32_t _id, const float _dx, const float _dest);
  void render_audio(const float _left, const float _right, const float _vol);
  void render_fast();
  void render_slow();
  void keyDown(PolyKeyEvent *_event);
  void flushDSP();
  void resetDSP();

 private:
  SmootherHandle<C15::Smoothers::Mono_Sync, C15::Smoothers::Mono_Audio, C15::Smoothers::Mono_Fast,
                 C15::Smoothers::Mono_Slow>
      m_smoothers;
  exponentiator *m_convert;
  LayerSignalCollection *m_z_self;
  Engine::Envelopes::DecayEnvelope<1> m_flanger_env;
  Engine::MonoFlanger m_flanger;
  Engine::MonoCabinet m_cabinet;
  Engine::MonoGapFilter m_gapfilter;
  Engine::MonoEcho m_echo;
  Engine::MonoReverb m_reverb;
  NlToolbox::Curves::Shaper_2_BP m_flanger_fb_curve;
  NlToolbox::Curves::Shaper_1_BP m_reverb_color_curve_1, m_reverb_color_curve_2;
  stereo_lfo m_flanger_lfo;
  const float m_flanger_norm_phase = 1.0f / 360.0f, m_cabinet_tilt_floor = 2.e-20f, m_delay_norm_stereo = 1.0f / 99.0f;
  float m_samplerate, m_reciprocal_samplerate, m_nyquist, m_millisecond, m_flanger_rate_to_decay = 0.55f;
  float evalNyquist(const float _value);
  void postProcess_audio();
  void postProcess_fast();
  void postProcess_slow();
};
