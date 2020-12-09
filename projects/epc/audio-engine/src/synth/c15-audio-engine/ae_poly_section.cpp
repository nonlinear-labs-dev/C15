#include "ae_poly_section.h"

/******************************************************************************/
/** @file       ae_poly_section.cpp
    @date
    @version    1.7-3
    @author     M. Seeber
    @brief      new container for all polyphonic parameters and dsp
    @todo
*******************************************************************************/

#include <cmath>
#include <nltools/logging/Log.h>

PolySection::PolySection()
{
}

void PolySection::init(GlobalSignals* _globalsignals, exponentiator* _convert, Engine::Handle::Time_Handle* _time,
                       LayerSignalCollection* _z_self, float* _reference, const float _ms, const float _gateRelease,
                       const float _samplerate, const uint32_t _upsampleFactor)
{
  // pointer linking (from dsp_host_dual)
  m_globalsignals = _globalsignals;
  m_convert = _convert;
  m_time = _time;
  m_z_self = _z_self;
  // init crucial variables
  m_reference = _reference;
  m_millisecond = _ms;  // redundant, m_time->m_millisecond could be used
  m_samplerate = _samplerate;
  m_nyquist = 0.5f * _samplerate;
  // init control shapers
  m_comb_decayCurve.setCurve(0.0f, 0.25f, 1.0f);
  m_svf_LBH1Curve.setCurve(-1.0f, -1.0f, 1.0f);
  m_svf_LBH2Curve.setCurve(-1.0f, 1.0f, 1.0f);
  m_svf_resCurve.setCurve(0.0f, 0.49f, 0.79f, 0.94f);
  // init dsp components
  m_env_g.init(1.0f / ((_ms * _gateRelease) + 1.0f));
  m_soundgenerator.init(m_voices, _samplerate);
  m_combfilter.init(_samplerate, _upsampleFactor);
  m_svfilter.init(_samplerate);
  m_feedbackmixer.init(_samplerate);
  m_outputmixer.init(_samplerate, C15::Config::local_polyphony);
  //
  resetVoiceFade();
#if POTENTIAL_IMPROVEMENT_GAIN_CURVE == __POTENTIAL_IMPROVEMENT_ENABLED__
  m_gain_curve_size = static_cast<uint32_t>(POTENTIAL_SETTING_GAIN_CURVE_TIME_MS * _ms);
  m_gain_curve_size_m1 = m_gain_curve_size - 1;
  m_gain_curve_buffer.resize(m_gain_curve_size);
  const float curve_norm = 1.0f / (static_cast<float>(m_gain_curve_size_m1));
  for(uint32_t curve_idx = 0; curve_idx < m_gain_curve_size; curve_idx++)
  {
    const float curve_phase = NlToolbox::Constants::pi * ((curve_norm * static_cast<float>(curve_idx)) - 0.5f),
                curve_val = std::sin(curve_phase);
    m_gain_curve_buffer[curve_idx] = 0.5f + (0.5f * curve_val);  // sine-based crossfade
  }
#endif
}

void PolySection::add_copy_sync_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_sync.add_copy_id(_smootherId, _signalId);
}

void PolySection::add_copy_audio_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_audio.add_copy_id(_smootherId, _signalId);
}

void PolySection::add_copy_fast_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_fast.add_copy_id(_smootherId, _signalId);
}

void PolySection::add_copy_slow_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_slow.add_copy_id(_smootherId, _signalId);
}

void PolySection::start_sync(const uint32_t _id, const float _dest)
{
  m_smoothers.start_sync(_id, _dest);
  if(m_smoothers.m_copy_sync.m_smootherId[_id])
  {
    m_signals.set_mono(m_smoothers.m_copy_sync.m_signalId[_id], _dest);
  }
}

void PolySection::start_audio(const uint32_t _id, const float _dx, const float _dest)
{
  m_smoothers.start_audio(_id, _dx, _dest);
}

void PolySection::start_fast(const uint32_t _id, const float _dx, const float _dest)
{
  m_smoothers.start_fast(_id, _dx, _dest);
}

void PolySection::start_slow(const uint32_t _id, const float _dx, const float _dest)
{
  m_smoothers.start_slow(_id, _dx, _dest);
}

void PolySection::render_audio(const float _mute)
{
  m_smoothers.render_audio();
  auto traversal = &m_smoothers.m_copy_audio;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set_mono(traversal->m_signalId[i], m_smoothers.get_audio(traversal->m_smootherId[i]));
  }
#if POTENTIAL_IMPROVEMENT_GAIN_CURVE == __POTENTIAL_IMPROVEMENT_DISABLED__
  for(uint32_t v = 0; v < m_voices; v++)
  {
    postProcess_poly_audio(v, _mute);
  }
  // render dsp components (former makepolysound)
  m_soundgenerator.generate(m_signals, m_feedbackmixer.m_out);
  m_combfilter.apply(m_signals, m_soundgenerator.m_out_A, m_soundgenerator.m_out_B);
  m_svfilter.apply(m_signals, m_soundgenerator.m_out_A, m_soundgenerator.m_out_B, m_combfilter.m_out);
  m_outputmixer.combine(m_signals, m_voice_level, m_soundgenerator.m_out_A, m_soundgenerator.m_out_B,
                        m_combfilter.m_out, m_svfilter.m_out);
#elif POTENTIAL_IMPROVEMENT_GAIN_CURVE == __POTENTIAL_IMPROVEMENT_ENABLED__
  PolyValue tmp_voice_level;
  for(uint32_t v = 0; v < m_voices; v++)
  {
    postProcess_poly_audio(v, _mute);
  }
  // as a separate voice loop without control flow branches, this loop should be SIMD-compliant
  for(uint32_t v = 0; v < m_voices; v++)
  {
    const uint32_t v_index = m_gain_curve_index[v];
    const float v_fade = m_gain_curve_buffer[v_index];
    tmp_voice_level[v] = ((1.0f - v_fade) * m_voice_level_old[v]) + (v_fade * m_voice_level[v]);
    m_gain_curve_index[v] = std::min(m_gain_curve_size_m1, v_index + 1);
  }
  // render dsp components (former makepolysound)
  m_soundgenerator.generate(m_signals, m_feedbackmixer.m_out);
  m_combfilter.apply(m_signals, m_soundgenerator.m_out_A, m_soundgenerator.m_out_B);
  m_svfilter.apply(m_signals, m_soundgenerator.m_out_A, m_soundgenerator.m_out_B, m_combfilter.m_out);
  m_outputmixer.combine(m_signals, tmp_voice_level, m_soundgenerator.m_out_A, m_soundgenerator.m_out_B,
                        m_combfilter.m_out, m_svfilter.m_out);
#endif
  m_outputmixer.filter_level(m_signals);
  // capture z samples
  m_z_self->m_osc_a = m_soundgenerator.m_out_A;
  m_z_self->m_osc_b = m_soundgenerator.m_out_B;
  m_z_self->m_comb = m_combfilter.m_out;
  m_z_self->m_svf = m_svfilter.m_out;
  // eval sends
  const float send_other = m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_To_FX), send_self = 1.0f - send_other;
  m_send_self_l = m_outputmixer.m_out_l * send_self;
  m_send_self_r = m_outputmixer.m_out_r * send_self;
  m_send_other_l = m_outputmixer.m_out_l * send_other;
  m_send_other_r = m_outputmixer.m_out_r * send_other;
  // env rendering (unfortunately another voice loop)
  for(uint32_t v = 0; v < m_voices; v++)
  {
    m_env_a.tick(v, _mute);
    m_env_b.tick(v, _mute);
    m_env_c.tick(v, _mute);
    m_env_g.tick(v);
  }
}

void PolySection::render_feedback(const LayerSignalCollection& _z_other)
{
  m_feedbackmixer.apply(m_signals, *m_z_self, _z_other);
  m_soundgenerator.m_feedback_phase_A = m_soundgenerator.m_feedback_phase_B = m_feedbackmixer.m_out;
}

void PolySection::render_fast()
{
  m_smoothers.render_fast();
  auto traversal = &m_smoothers.m_copy_fast;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set_mono(traversal->m_signalId[i], m_smoothers.get_fast(traversal->m_smootherId[i]));
  }
  postProcess_mono_fast();
  for(uint32_t v = 0; v < m_voices; v++)
  {
    postProcess_poly_fast(v);
  }
}

void PolySection::render_slow()
{
  m_smoothers.render_slow();
  // update glide time and render glide smoother
  m_mono_glide.timeUpdate(0, m_time->eval_ms(3, m_smoothers.get(C15::Smoothers::Poly_Slow::Mono_Grp_Glide)));
  m_mono_glide.render();
  auto traversal = &m_smoothers.m_copy_slow;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set_mono(traversal->m_signalId[i], m_smoothers.get_slow(traversal->m_smootherId[i]));
  }
  postProcess_mono_slow();
  for(uint32_t v = 0; v < m_voices; v++)
  {
    updateNotePitch(v);
    postProcess_poly_slow(v);
    setSlowFilterCoefs(v);
  }
}

bool PolySection::keyDown(PolyKeyEvent* _event)
{
  const bool retrigger_mono
      = (m_key_active == 0),
      rstA = _event->m_trigger_env && static_cast<bool>(m_signals.get(C15::Signals::Quasipoly_Signals::Osc_A_Reset)),
      rstB = _event->m_trigger_env && static_cast<bool>(m_signals.get(C15::Signals::Quasipoly_Signals::Osc_B_Reset));
  m_unison_index[_event->m_voiceId] = _event->m_unisonIndex;
  m_last_key_tune[_event->m_voiceId] = m_base_pitch[_event->m_voiceId];
  m_key_tune[_event->m_voiceId] = _event->m_tune;
  m_key_position[_event->m_voiceId] = _event->m_position;
  if(_event->m_allow_glide)
  {
    if(_event->m_trigger_glide)
    {
      m_mono_glide.sync(0, 0.0f);
      m_mono_glide.start(0, m_time->eval_ms(3, m_smoothers.get(C15::Smoothers::Poly_Slow::Mono_Grp_Glide)), 1.0f);
      m_mono_glide.render();
    }
    else
    {
      m_mono_glide.sync(0, 1.0f);
    }
    postProcess_mono_slow();
  }
  //  if(_event->m_stolen)
  //  {
  //    /* case NoteSteal (currently nothing) */
  //  }
  m_soundgenerator.resetPhase(_event->m_voiceId, rstA, rstB);
  updateNotePitch(_event->m_voiceId);
  // envelope (re)starts and new voice levels only in non-legato cases
  if(_event->m_trigger_env)
  {
#if POTENTIAL_IMPROVEMENT_GAIN_CURVE == __POTENTIAL_IMPROVEMENT_DISABLED__
    m_voice_level[_event->m_voiceId] = m_key_levels[_event->m_position];
#elif POTENTIAL_IMPROVEMENT_GAIN_CURVE == __POTENTIAL_IMPROVEMENT_ENABLED__
    m_voice_level_old[_event->m_voiceId] = m_voice_level[_event->m_voiceId];
    m_voice_level[_event->m_voiceId] = m_key_levels[_event->m_position];
    m_gain_curve_index[_event->m_voiceId] = 0;
#endif
    startEnvelopes(_event->m_voiceId, m_note_pitch[_event->m_voiceId], _event->m_velocity);
  }
  // process signals after starting envelopes
  postProcess_poly_key(_event->m_voiceId);
  setSlowFilterCoefs(_event->m_voiceId);
  // set comb delay smoother after pitch processing (unfortunately separate if statement)
  // is this legato-dependant?
  if(_event->m_trigger_env)
  {
    m_combfilter.setDelaySmoother(_event->m_voiceId);
  }
  m_key_active++;
  return retrigger_mono;
}

void PolySection::keyUp(PolyKeyEvent* _event)
{
  m_last_key_tune[_event->m_voiceId] = m_base_pitch[_event->m_voiceId];
  m_key_tune[_event->m_voiceId] = _event->m_tune;
  m_key_position[_event->m_voiceId] = _event->m_position;
  if(_event->m_allow_glide)
  {
    if(_event->m_trigger_glide)
    {
      m_mono_glide.sync(0, 0.0f);
      m_mono_glide.start(0, m_time->eval_ms(3, m_smoothers.get(C15::Smoothers::Poly_Slow::Mono_Grp_Glide)), 1.0f);
    }
    else
    {
      m_mono_glide.sync(0, 1.0f);
    }
  }
  updateNotePitch(_event->m_voiceId);
  if(_event->m_trigger_env)
  {
    stopEnvelopes(_event->m_voiceId, m_note_pitch[_event->m_voiceId], _event->m_velocity);
  }
  m_key_active--;
}

void PolySection::resetEnvelopes()
{
  m_env_a.reset();
  m_env_b.reset();
  m_env_c.reset();
  m_env_g.reset();
}

void PolySection::flushDSP()
{
  m_soundgenerator.resetDSP();
  m_combfilter.resetDSP();
  m_svfilter.resetDSP();
  m_feedbackmixer.resetDSP();
  m_outputmixer.resetDSP();
}

void PolySection::resetDSP()
{
  resetEnvelopes();
  flushDSP();
  m_send_self_l = m_send_self_r = m_send_other_l = m_send_other_r = 0.0f;
}

float PolySection::getVoiceGroupVolume()
{
  return m_smoothers.get(C15::Smoothers::Poly_Fast::Voice_Grp_Volume)
      * m_smoothers.get(C15::Smoothers::Poly_Fast::Voice_Grp_Mute);
}

void PolySection::evalVoiceFade(const float _from, const float _range)
{
  // floating approach: the number of faded keys is ceil(_range), the fade slope is depending on floating range
  // (previously, faded keys were floor(_range), the fade slope was depending on floor(_range)
  // potential problem: gain in precision is hardly audible (even with other part muted), so is perceived general improvement

  const int32_t start = m_fadeStart;
  const int32_t fadeStart
      = start + (m_fadeIncrement * (1 + (m_fadeStart + (m_fadeIncrement * static_cast<int32_t>(_from)))));
  const int32_t fadeEnd = fadeStart + (m_fadeIncrement * static_cast<int32_t>(std::ceil(_range)));
  const float slope = 1.0f / (_range + 1.0f);
  float fade = 1.0f;

  // phase 1: 100%
  for(int32_t i = start; i != fadeStart; i += m_fadeIncrement)
  {
    if((i < 0) || (i >= C15::Config::virtual_key_count))
      break;  // safety mechanism

    m_key_levels[i] = 1.0f;
  }

  // phase 2: fade out
  for(int32_t i = fadeStart; i != fadeEnd; i += m_fadeIncrement)
  {
    if((i < 0) || (i >= C15::Config::virtual_key_count))
      break;  // safety mechanism

    fade -= slope;

    // linear implementation (not smooth enough):
    //float fadeValue = fade;

    // sine implementation (keep for further testing):
    //float fadeValue = 0.5f - (0.5f * NlToolbox::Math::sinP3_wrap(0.5f * (fade - 0.5f)));

    // parabolic implementation (currently favourite):
    float fadeValue = fade * fade;
    if(fade < 0.5f)
    {
      fadeValue = 2.0f * fadeValue;
    }
    else
    {
      fadeValue = (-2.0f * fadeValue) + (4.0f * fade) - 1.0f;
    }

    m_key_levels[i] = fadeValue;
  }

  // phase 3: 0%
  for(int32_t i = fadeEnd; i != m_fadeEnd + m_fadeIncrement; i += m_fadeIncrement)
  {
    if((i < 0) || (i >= C15::Config::virtual_key_count))
      break;  // safety mechanism

    m_key_levels[i] = 0.0f;
  }
  //  testVoiceFadeTable();
}

void PolySection::evalVoiceFadeInterpolated(const float _from, const float _range)
{
  // floating and interpolated approach: the number of faded keys is ceil(_range), ...
  // ... fade slope is now interpolated (determined by _range), using both floor(_range) and ceil(_range) as separate slope ramps for interpolation, ...
  // ... generating slightly larger fade ramp values when _range has fractional part
  // (previously, approach was floating only)
  // potential improvement: gain in precision should be more audible, so should perceived general improvement

  const int32_t start = m_fadeStart,
                fadeStart = start + (m_fadeIncrement * (1 + start + (m_fadeIncrement * static_cast<int32_t>(_from)))),
                upperRange = static_cast<int32_t>(std::ceil(_range)),
                lowerRange = static_cast<int32_t>(std::floor(_range)),
                fadeEnd = fadeStart + (m_fadeIncrement * upperRange);

  // as described, we now evaluate two separate ramps with different slopes and interpolate by crossfading according to the fractional
  const float fractional = _range - static_cast<float>(lowerRange),
              slope[2]
      = { (1.0f / (static_cast<float>(lowerRange) + 1.0f)), (1.0f / (static_cast<float>(upperRange) + 1.0f)) };
  float fade[2] = { 1.0f, 1.0f };

  // phase 1: 100%
  for(int32_t i = start; i != fadeStart; i += m_fadeIncrement)
  {
    if((i < 0) || (i >= C15::Config::virtual_key_count))
      break;  // safety mechanism

    m_key_levels[i] = 1.0f;
  }

  // phase 2: fade out
  for(int32_t i = fadeStart; i != fadeEnd; i += m_fadeIncrement)
  {
    if((i < 0) || (i >= C15::Config::virtual_key_count))
      break;  // safety mechanism

    fade[0] -= slope[0];
    fade[1] -= slope[1];

    // linear implementation (not smooth enough):
    //    float fadeValue[2] = { fade[0], fade[1] };

    // sine implementation (keep for further testing):
    //    float fadeValue[2] = { (0.5f - (0.5f * NlToolbox::Math::sinP3_wrap(0.5f * (fade[0] - 0.5f)))),
    //                           (0.5f - (0.5f * NlToolbox::Math::sinP3_wrap(0.5f * (fade[1] - 0.5f)))) };

    // parabolic implementation (currently favourite):
    float fadeValue[2] = { (fade[0] * fade[0]), (fade[1] * fade[1]) };
    for(uint32_t fi = 0; fi < 2; fi++)
    {
      if(fade[fi] < 0.5f)
      {
        fadeValue[fi] = 2.0f * fadeValue[fi];
      }
      else
      {
        fadeValue[fi] = (-2.0f * fadeValue[fi]) + (4.0f * fade[fi]) - 1.0f;
      }
    }

    m_key_levels[i] = ((1.0f - fractional) * fadeValue[0]) + (fractional * fadeValue[1]);  // now interpolated
  }

  // phase 3: 0%
  for(int32_t i = fadeEnd; i != m_fadeEnd + m_fadeIncrement; i += m_fadeIncrement)
  {
    if((i < 0) || (i >= C15::Config::virtual_key_count))
      break;  // safety mechanism

    m_key_levels[i] = 0.0f;
  }
  //  testVoiceFadeTable();
}

void PolySection::resetVoiceFade()
{
  for(uint32_t i = 0; i < C15::Config::virtual_key_count; i++)
  {
    m_key_levels[i] = 1.0f;
  }
}

float PolySection::evalNyquist(const float _value)
{
  return _value > m_nyquist ? m_nyquist : _value;
}

float PolySection::evalScale(const uint32_t _voiceId)
{
  // adding one octave offset in order to avoid negative numbers
  const int32_t pos = static_cast<int32_t>(m_key_position[_voiceId] + 12);
  // subtract base key -- twice
  const int32_t pos_start
      = pos - static_cast<int32_t>(m_globalsignals->get(C15::Signals::Global_Signals::Scale_Base_Key_Start));
  const int32_t pos_dest
      = pos - static_cast<int32_t>(m_globalsignals->get(C15::Signals::Global_Signals::Scale_Base_Key_Dest));
  // form scale offset index by signal offset + cyclic index -- twice
  const uint32_t index_start
      = static_cast<uint32_t>(C15::Signals::Global_Signals::Scale_Offset_0) + (static_cast<uint32_t>(pos_start) % 12);
  const uint32_t index_dest
      = static_cast<uint32_t>(C15::Signals::Global_Signals::Scale_Offset_0) + (static_cast<uint32_t>(pos_dest) % 12);
  // retrieve base key smoother fade position and crossfade offsets
  const float fade = m_globalsignals->get(C15::Signals::Global_Signals::Scale_Base_Key_Pos);
  return ((1.0f - fade) * m_globalsignals->get(index_start)) + (fade * m_globalsignals->get(index_dest));
}

void PolySection::postProcess_poly_audio(const uint32_t _voiceId, const float _mute)
{
  // provide poly env signals
  float gain = m_smoothers.get(C15::Smoothers::Poly_Fast::Env_A_Gain);
  m_signals.set_poly(C15::Signals::Truepoly_Signals::Env_A_Mag, _voiceId,
                     gain * m_env_a.m_body[_voiceId].m_signal_magnitude);
  m_signals.set_poly(C15::Signals::Truepoly_Signals::Env_A_Tmb, _voiceId,
                     gain * m_env_a.m_body[_voiceId].m_signal_timbre);
  gain = m_smoothers.get(C15::Smoothers::Poly_Fast::Env_B_Gain);
  m_signals.set_poly(C15::Signals::Truepoly_Signals::Env_B_Mag, _voiceId,
                     gain * m_env_b.m_body[_voiceId].m_signal_magnitude);
  m_signals.set_poly(C15::Signals::Truepoly_Signals::Env_B_Tmb, _voiceId,
                     gain * m_env_b.m_body[_voiceId].m_signal_timbre);
  m_signals.set_poly(C15::Signals::Truepoly_Signals::Env_C_Clip, _voiceId, m_env_c.m_body[_voiceId].m_signal_magnitude);
  m_signals.set_poly(C15::Signals::Truepoly_Signals::Env_C_Uncl, _voiceId,
                     m_env_c.m_body[_voiceId].m_signal_magnitude * m_env_c.m_clipFactor[_voiceId]);
  m_signals.set_poly(C15::Signals::Truepoly_Signals::Env_G_Sig, _voiceId, m_env_g.m_body[_voiceId].m_signal_magnitude);
  // temporary variables
  float tmp_amt, tmp_env;
  // poly osc a signals
  tmp_amt = m_smoothers.get(C15::Smoothers::Poly_Fast::Osc_A_PM_Self);
  tmp_env = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_A_PM_Self_Env_A);
  m_signals.set(C15::Signals::Truepoly_Signals::Osc_A_PM_Self_Env_A, _voiceId,
                NlToolbox::Crossfades::unipolarCrossFade(
                    1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_A_Tmb, _voiceId), tmp_env)
                    * tmp_amt);
  tmp_amt = m_smoothers.get(C15::Smoothers::Poly_Fast::Osc_A_PM_B);
  tmp_env = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_A_PM_B_Env_B);
  m_signals.set(C15::Signals::Truepoly_Signals::Osc_A_PM_B_Env_B, _voiceId,
                NlToolbox::Crossfades::unipolarCrossFade(
                    1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_B_Tmb, _voiceId), tmp_env)
                    * tmp_amt);
  tmp_amt = m_smoothers.get(C15::Smoothers::Poly_Fast::Osc_A_PM_FB);
  tmp_env = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_A_PM_FB_Env_C);
  m_signals.set(C15::Signals::Truepoly_Signals::Osc_A_PM_FB_Env_C, _voiceId,
                NlToolbox::Crossfades::unipolarCrossFade(
                    1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Clip, _voiceId), tmp_env)
                    * tmp_amt);
  // poly shp a signals
  tmp_amt = m_smoothers.get(C15::Smoothers::Poly_Fast::Shp_A_Drive);
  tmp_env = m_smoothers.get(C15::Smoothers::Poly_Slow::Shp_A_Drive_Env_A);
  m_signals.set(C15::Signals::Truepoly_Signals::Shp_A_Drive_Env_A, _voiceId,
                (NlToolbox::Crossfades::unipolarCrossFade(
                     1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_A_Tmb, _voiceId), tmp_env)
                 * tmp_amt)
                    + 0.18f);
  tmp_env = m_smoothers.get(C15::Smoothers::Poly_Slow::Shp_A_FB_Env_C);
  m_signals.set(C15::Signals::Truepoly_Signals::Shp_A_FB_Env_C, _voiceId,
                NlToolbox::Crossfades::unipolarCrossFade(
                    m_signals.get(C15::Signals::Truepoly_Signals::Env_G_Sig, _voiceId),
                    m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Clip, _voiceId), tmp_env));
  // poly osc b signals
  tmp_amt = m_smoothers.get(C15::Smoothers::Poly_Fast::Osc_B_PM_Self);
  tmp_env = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_B_PM_Self_Env_B);
  m_signals.set(C15::Signals::Truepoly_Signals::Osc_B_PM_Self_Env_B, _voiceId,
                NlToolbox::Crossfades::unipolarCrossFade(
                    1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_B_Tmb, _voiceId), tmp_env)
                    * tmp_amt);
  tmp_amt = m_smoothers.get(C15::Smoothers::Poly_Fast::Osc_B_PM_A);
  tmp_env = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_B_PM_A_Env_A);
  m_signals.set(C15::Signals::Truepoly_Signals::Osc_B_PM_A_Env_A, _voiceId,
                NlToolbox::Crossfades::unipolarCrossFade(
                    1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_A_Tmb, _voiceId), tmp_env)
                    * tmp_amt);
  tmp_amt = m_smoothers.get(C15::Smoothers::Poly_Fast::Osc_B_PM_FB);
  tmp_env = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_B_PM_FB_Env_C);
  m_signals.set(C15::Signals::Truepoly_Signals::Osc_B_PM_FB_Env_C, _voiceId,
                NlToolbox::Crossfades::unipolarCrossFade(
                    1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Clip, _voiceId), tmp_env)
                    * tmp_amt);
  // poly shp b signals
  tmp_amt = m_smoothers.get(C15::Smoothers::Poly_Fast::Shp_B_Drive);
  tmp_env = m_smoothers.get(C15::Smoothers::Poly_Slow::Shp_B_Drive_Env_B);
  m_signals.set(C15::Signals::Truepoly_Signals::Shp_B_Drive_Env_B, _voiceId,
                (NlToolbox::Crossfades::unipolarCrossFade(
                     1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_B_Tmb, _voiceId), tmp_env)
                 * tmp_amt)
                    + 0.18f);
  tmp_env = m_smoothers.get(C15::Smoothers::Poly_Slow::Shp_B_FB_Env_C);
  m_signals.set(C15::Signals::Truepoly_Signals::Shp_B_FB_Env_C, _voiceId,
                NlToolbox::Crossfades::unipolarCrossFade(
                    m_signals.get(C15::Signals::Truepoly_Signals::Env_G_Sig, _voiceId),
                    m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Clip, _voiceId), tmp_env));
  // comb filter
  tmp_amt = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_Pitch_Env_C);
  tmp_env = m_convert->eval_lin_pitch(
      69.0f - (tmp_amt * m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Uncl, _voiceId)));
  m_signals.set(C15::Signals::Truepoly_Signals::Comb_Flt_Freq_Env_C, _voiceId, tmp_env);
  tmp_env = m_signals.get(C15::Signals::Truepoly_Signals::Env_G_Sig,
                          _voiceId);  // application of gate signal (with 10ms release)
  tmp_amt = NlToolbox::Crossfades::unipolarCrossFade(m_comb_decay_times[0][_voiceId], m_comb_decay_times[1][_voiceId],
                                                     tmp_env);
  m_signals.set(C15::Signals::Truepoly_Signals::Comb_Flt_Decay, _voiceId,
                tmp_amt);  // decay now fully formed as min/max
  // unison (phase)
  const float phase = m_smoothers.get(C15::Smoothers::Poly_Audio::Unison_Phase)
      * m_spread.m_phase[m_uVoice][m_unison_index[_voiceId]];
  m_signals.set(C15::Signals::Truepoly_Signals::Unison_PolyPhase, _voiceId, phase);
}

void PolySection::postProcess_poly_fast(const uint32_t _voiceId)
{
  updateEnvLevels(_voiceId);
#if POTENTIAL_IMPROVEMENT_FAST_PITCH_PROCESSING == __POTENTIAL_IMPROVEMENT_ENABLED__
  postProcess_poly_pitch(_voiceId, m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Uncl, _voiceId));
#endif
  // temporary variables
  const uint32_t uIndex = m_unison_index[_voiceId];
  const float basePitch = m_base_pitch[_voiceId], notePitch = m_note_pitch[_voiceId];
  float tmp_lvl, tmp_pan;
  // feedback mixer
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::FB_Mix_Lvl);
  tmp_pan = std::min(m_convert->eval_level(m_smoothers.get(C15::Smoothers::Poly_Slow::FB_Mix_Lvl_KT) * notePitch),
                     env_clip_peak);
  m_signals.set(C15::Signals::Truepoly_Signals::FB_Mix_Lvl, _voiceId, tmp_lvl * tmp_pan);
  // output mixer
  const float poly_pan = m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_Key_Pan) * (basePitch - 6.0f)
      + (m_smoothers.get(C15::Smoothers::Poly_Fast::Unison_Pan) * m_spread.m_pan[m_uVoice][uIndex]);
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_A_Lvl);
  tmp_pan = std::clamp(m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_A_Pan) + poly_pan, 0.0f, 1.0f);
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_A_Left, _voiceId, tmp_lvl * (1.0f - tmp_pan));
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_A_Right, _voiceId, tmp_lvl * tmp_pan);
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_B_Lvl);
  tmp_pan = std::clamp(m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_B_Pan) + poly_pan, 0.0f, 1.0f);
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_B_Left, _voiceId, tmp_lvl * (1.0f - tmp_pan));
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_B_Right, _voiceId, tmp_lvl * tmp_pan);
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_Comb_Lvl);
  tmp_pan = std::clamp(m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_Comb_Pan) + poly_pan, 0.0f, 1.0f);
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_Comb_Left, _voiceId, tmp_lvl * (1.0f - tmp_pan));
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_Comb_Right, _voiceId, tmp_lvl * tmp_pan);
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_SVF_Lvl);
  tmp_pan = std::clamp(m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_SVF_Pan) + poly_pan, 0.0f, 1.0f);
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_SVF_Left, _voiceId, tmp_lvl * (1.0f - tmp_pan));
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_SVF_Right, _voiceId, tmp_lvl * tmp_pan);
}

void PolySection::postProcess_mono_fast()
{
  // temporary variables
  float tmp_lvl, tmp_abs, tmp_inv;
  // state variable filter
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::SV_Flt_LBH);
  m_signals.set(C15::Signals::Quasipoly_Signals::SV_Flt_LBH_1, m_svf_LBH1Curve.applyCurve(tmp_lvl));
  m_signals.set(C15::Signals::Quasipoly_Signals::SV_Flt_LBH_2, m_svf_LBH2Curve.applyCurve(tmp_lvl));
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::SV_Flt_Par);
  tmp_abs = std::abs(tmp_lvl);
  tmp_inv = 1.0f - tmp_abs;
  m_signals.set(C15::Signals::Quasipoly_Signals::SV_Flt_Par_1, 0.7f * tmp_abs);
  m_signals.set(C15::Signals::Quasipoly_Signals::SV_Flt_Par_2, (0.7f * tmp_lvl) + tmp_inv);
  m_signals.set(C15::Signals::Quasipoly_Signals::SV_Flt_Par_3, tmp_inv);
  m_signals.set(C15::Signals::Quasipoly_Signals::SV_Flt_Par_4, tmp_abs);
}

void PolySection::postProcess_poly_slow(const uint32_t _voiceId)
{
  // envelopes
  updateEnvTimes(_voiceId);
#if POTENTIAL_IMPROVEMENT_FAST_PITCH_PROCESSING == __POTENTIAL_IMPROVEMENT_DISABLED__
  postProcess_poly_pitch(_voiceId, m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Uncl, _voiceId));
#endif
  // temporary variables
  const float notePitch = m_note_pitch[_voiceId];
  float keyTracking, unitPitch, envMod, unitSign, unitMod, unitValue;
  // osc a
  envMod = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_A_Fluct_Env_C);
  m_signals.set(C15::Signals::Truepoly_Signals::Osc_A_Fluct_Env_C, _voiceId,
                m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_A_Fluct)
                    * NlToolbox::Crossfades::unipolarCrossFade(
                          1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Clip, _voiceId), envMod));
  // osc b
  envMod = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_B_Fluct_Env_C);
  m_signals.set(C15::Signals::Truepoly_Signals::Osc_B_Fluct_Env_C, _voiceId,
                m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_B_Fluct)
                    * NlToolbox::Crossfades::unipolarCrossFade(
                          1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Clip, _voiceId), envMod));
  // comb filter
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_Decay_KT);
  unitSign = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_Decay) < 0 ? -0.001f : 0.001f;
  envMod = 1.0f - m_comb_decayCurve.applyCurve(m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_Decay_Gate));
  unitMod = std::abs(m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_Decay));
  unitPitch = (-0.5f * notePitch * keyTracking);
  unitValue
      = m_signals.get(C15::Signals::Truepoly_Signals::Comb_Flt_Freq)[_voiceId];  // decay now fully formed as min/max
  m_comb_decay_times[0][_voiceId]
      = m_combfilter.calcDecayGain(m_convert->eval_level(unitPitch + (unitMod * envMod)) * unitSign, unitValue);
  m_comb_decay_times[1][_voiceId]
      = m_combfilter.calcDecayGain(m_convert->eval_level(unitPitch + unitMod) * unitSign, unitValue);
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_AP_KT);
  unitPitch = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_AP_Tune);
  envMod = m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Uncl, _voiceId)
      * m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_AP_Env_C);
  m_signals.set(
      C15::Signals::Truepoly_Signals::Comb_Flt_AP_Freq, _voiceId,
      evalNyquist(440.0f * unitPitch * m_convert->eval_lin_pitch(69.0f + (notePitch * keyTracking) + envMod)));
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_LP_KT);
  unitPitch = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_LP_Tune);
  envMod = m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Uncl, _voiceId)
      * m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_LP_Env_C);
  m_signals.set(
      C15::Signals::Truepoly_Signals::Comb_Flt_LP_Freq, _voiceId,
      evalNyquist(440.0f * unitPitch * m_convert->eval_lin_pitch(69.0f + (notePitch * keyTracking) + envMod)));
  // state variable filter
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::SV_Flt_Res_KT) * m_svf_resFactor;
  envMod = m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Clip, _voiceId)
      * m_smoothers.get(C15::Smoothers::Poly_Slow::SV_Flt_Res_Env_C);
  unitPitch = m_svf_resCurve.applyCurve(std::clamp(
      m_smoothers.get(C15::Smoothers::Poly_Slow::SV_Flt_Res) + envMod + (notePitch * keyTracking), 0.0f, 1.0f));
  m_signals.set(C15::Signals::Truepoly_Signals::SV_Flt_Res_Damp, _voiceId, 2.0f - (2.0f * unitPitch));
  m_signals.set(C15::Signals::Truepoly_Signals::SV_Flt_Res_FMax, _voiceId,
                0.7352f + (0.2930f * unitPitch * (1.3075f + unitPitch)));
  // feedback mixer
  m_signals.set(C15::Signals::Truepoly_Signals::FB_Mix_HPF, _voiceId,
                evalNyquist(m_convert->eval_lin_pitch(12.0f + notePitch) * 440.0f));
}

void PolySection::postProcess_mono_slow()
{
  // osc a
  m_signals.set(C15::Signals::Quasipoly_Signals::Osc_A_Chirp,
                evalNyquist(m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_A_Chirp) * 440.0f));
  // osc b
  m_signals.set(C15::Signals::Quasipoly_Signals::Osc_B_Chirp,
                evalNyquist(m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_B_Chirp) * 440.0f));
}

void PolySection::postProcess_poly_pitch(const uint32_t _voiceId, const float _envC)
{
  const float notePitch = m_note_pitch[_voiceId];
  float keyTracking, unitPitch, envMod, unitValue, unitSpread, unitMod;
  // osc a (pitch only)
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_A_Pitch_KT);
  unitPitch = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_A_Pitch);
  envMod = _envC * m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_A_Pitch_Env_C);
  m_signals.set(
      C15::Signals::Truepoly_Signals::Osc_A_Freq, _voiceId,
      evalNyquist((*m_reference) * m_convert->eval_osc_pitch(unitPitch + (notePitch * keyTracking) + envMod)));
  // osc b (pitch only)
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_B_Pitch_KT);
  unitPitch = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_B_Pitch);
  envMod = _envC * m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_B_Pitch_Env_C);
  m_signals.set(
      C15::Signals::Truepoly_Signals::Osc_B_Freq, _voiceId,
      evalNyquist((*m_reference) * m_convert->eval_osc_pitch(unitPitch + (notePitch * keyTracking) + envMod)));
  // comb filter (pitch_as_delay_time and comb_bypass only) - maybe, take ap filter freq into account as well?
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_Pitch_KT);
  unitPitch = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_Pitch);
  m_signals.set(C15::Signals::Truepoly_Signals::Comb_Flt_Freq, _voiceId,
                evalNyquist((*m_reference) * unitPitch * m_convert->eval_lin_pitch(69.0f + (notePitch * keyTracking))));
  m_signals.set(C15::Signals::Truepoly_Signals::Comb_Flt_Bypass, _voiceId,
                unitPitch > dsp_comb_max_freqFactor ? 1.0f : 0.0f);
  // state variable filter (lower and upper cut and fm only)
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::SV_Flt_Cut_KT);
  envMod = _envC * m_smoothers.get(C15::Smoothers::Poly_Slow::SV_Flt_Cut_Env_C);
  unitPitch = (*m_reference) * m_smoothers.get(C15::Smoothers::Poly_Slow::SV_Flt_Cut);
  unitSpread = m_smoothers.get(C15::Smoothers::Poly_Slow::SV_Flt_Spread);
  unitMod = m_smoothers.get(C15::Smoothers::Poly_Slow::SV_Flt_FM);
  unitValue
      = evalNyquist(unitPitch * m_convert->eval_lin_pitch(69.0f + (notePitch * keyTracking) + envMod + unitSpread));
  m_signals.set(C15::Signals::Truepoly_Signals::SV_Flt_F1_Cut, _voiceId, unitValue);
  m_signals.set(C15::Signals::Truepoly_Signals::SV_Flt_F1_FM, _voiceId, unitValue * unitMod);
  unitValue
      = evalNyquist(unitPitch * m_convert->eval_lin_pitch(69.0f + (notePitch * keyTracking) + envMod - unitSpread));
  m_signals.set(C15::Signals::Truepoly_Signals::SV_Flt_F2_Cut, _voiceId, unitValue);
  m_signals.set(C15::Signals::Truepoly_Signals::SV_Flt_F2_FM, _voiceId, unitValue * unitMod);
  // maybe take fb mix hp into account as well?
}

void PolySection::postProcess_poly_key(const uint32_t _voiceId)
{
  postProcess_poly_pitch(_voiceId, m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Uncl, _voiceId));
  // - key override event now fully separated from clock, env c peak level is active immediately when attack == 0
  // LATER: consider re-formulating envelopes (keydown already affects signal) and rendering them after everything else (avoiding this override)
  // pitch, unison, temporary variables
  const uint32_t uIndex = m_unison_index[_voiceId];
  const float basePitch = m_base_pitch[_voiceId], notePitch = m_note_pitch[_voiceId];
  float keyTracking, unitPitch, envMod, unitSign, unitMod, unitValue, tmp_lvl, tmp_pan;
  // osc a
  envMod = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_A_Fluct_Env_C);
  m_signals.set(C15::Signals::Truepoly_Signals::Osc_A_Fluct_Env_C, _voiceId,
                m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_A_Fluct)
                    * NlToolbox::Crossfades::unipolarCrossFade(
                          1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Clip, _voiceId), envMod));
  // osc b
  envMod = m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_B_Fluct_Env_C);
  m_signals.set(C15::Signals::Truepoly_Signals::Osc_B_Fluct_Env_C, _voiceId,
                m_smoothers.get(C15::Smoothers::Poly_Slow::Osc_B_Fluct)
                    * NlToolbox::Crossfades::unipolarCrossFade(
                          1.0f, m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Clip, _voiceId), envMod));
  // comb filter
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_Decay_KT);
  unitSign = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_Decay) < 0 ? -0.001f : 0.001f;
  envMod = 1.0f - m_comb_decayCurve.applyCurve(m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_Decay_Gate));
  unitMod = std::abs(m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_Decay));
  unitPitch = (-0.5f * notePitch * keyTracking);
  unitValue
      = m_signals.get(C15::Signals::Truepoly_Signals::Comb_Flt_Freq)[_voiceId];  // decay now fully formed as min/max
  m_comb_decay_times[0][_voiceId]
      = m_combfilter.calcDecayGain(m_convert->eval_level(unitPitch + (unitMod * envMod)) * unitSign, unitValue);
  m_comb_decay_times[1][_voiceId]
      = m_combfilter.calcDecayGain(m_convert->eval_level(unitPitch + unitMod) * unitSign, unitValue);
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_AP_KT);
  unitPitch = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_AP_Tune);
  envMod = m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Uncl, _voiceId)
      * m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_AP_Env_C);
  m_signals.set(
      C15::Signals::Truepoly_Signals::Comb_Flt_AP_Freq, _voiceId,
      evalNyquist(440.0f * unitPitch * m_convert->eval_lin_pitch(69.0f + (notePitch * keyTracking) + envMod)));
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_LP_KT);
  unitPitch = m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_LP_Tune);
  envMod = m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Uncl, _voiceId)
      * m_smoothers.get(C15::Smoothers::Poly_Slow::Comb_Flt_LP_Env_C);
  m_signals.set(
      C15::Signals::Truepoly_Signals::Comb_Flt_LP_Freq, _voiceId,
      evalNyquist(440.0f * unitPitch * m_convert->eval_lin_pitch(69.0f + (notePitch * keyTracking) + envMod)));
  // state variable filter
  keyTracking = m_smoothers.get(C15::Smoothers::Poly_Slow::SV_Flt_Res_KT) * m_svf_resFactor;
  envMod = m_signals.get(C15::Signals::Truepoly_Signals::Env_C_Clip, _voiceId)
      * m_smoothers.get(C15::Smoothers::Poly_Slow::SV_Flt_Res_Env_C);
  unitPitch = m_svf_resCurve.applyCurve(std::clamp(
      m_smoothers.get(C15::Smoothers::Poly_Slow::SV_Flt_Res) + envMod + (notePitch * keyTracking), 0.0f, 1.0f));
  m_signals.set(C15::Signals::Truepoly_Signals::SV_Flt_Res_Damp, _voiceId, 2.0f - (2.0f * unitPitch));
  m_signals.set(C15::Signals::Truepoly_Signals::SV_Flt_Res_FMax, _voiceId,
                0.7352f + (0.2930f * unitPitch * (1.3075f + unitPitch)));
  // feedback mixer
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::FB_Mix_Lvl);
  tmp_pan = std::min(m_convert->eval_level(m_smoothers.get(C15::Smoothers::Poly_Slow::FB_Mix_Lvl_KT) * notePitch),
                     env_clip_peak);
  m_signals.set(C15::Signals::Truepoly_Signals::FB_Mix_Lvl, _voiceId, tmp_lvl * tmp_pan);
  m_signals.set(C15::Signals::Truepoly_Signals::FB_Mix_HPF, _voiceId,
                evalNyquist(m_convert->eval_lin_pitch(12.0f + notePitch) * 440.0f));
  // output mixer
  const float poly_pan = m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_Key_Pan) * (basePitch - 6.0f)
      + (m_smoothers.get(C15::Smoothers::Poly_Fast::Unison_Pan) * m_spread.m_pan[m_uVoice][uIndex]);
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_A_Lvl);
  tmp_pan = std::clamp(m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_A_Pan) + poly_pan, 0.0f, 1.0f);
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_A_Left, _voiceId, tmp_lvl * (1.0f - tmp_pan));
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_A_Right, _voiceId, tmp_lvl * tmp_pan);
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_B_Lvl);
  tmp_pan = std::clamp(m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_B_Pan) + poly_pan, 0.0f, 1.0f);
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_B_Left, _voiceId, tmp_lvl * (1.0f - tmp_pan));
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_B_Right, _voiceId, tmp_lvl * tmp_pan);
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_Comb_Lvl);
  tmp_pan = std::clamp(m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_Comb_Pan) + poly_pan, 0.0f, 1.0f);
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_Comb_Left, _voiceId, tmp_lvl * (1.0f - tmp_pan));
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_Comb_Right, _voiceId, tmp_lvl * tmp_pan);
  tmp_lvl = m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_SVF_Lvl);
  tmp_pan = std::clamp(m_smoothers.get(C15::Smoothers::Poly_Fast::Out_Mix_SVF_Pan) + poly_pan, 0.0f, 1.0f);
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_SVF_Left, _voiceId, tmp_lvl * (1.0f - tmp_pan));
  m_signals.set(C15::Signals::Truepoly_Signals::Out_Mix_SVF_Right, _voiceId, tmp_lvl * tmp_pan);
}

void PolySection::startEnvelopes(const uint32_t _voiceId, const float _pitch, const float _vel)
{
  float time, timeKT, dest, levelVel, attackVel, decay1Vel, decay2Vel, levelKT, peak, unclipped;
  // env a
  timeKT = -0.5f * m_smoothers.get(C15::Smoothers::Poly_Sync::Env_A_Time_KT) * _pitch;
  levelVel = m_smoothers.get(C15::Smoothers::Poly_Sync::Env_A_Lvl_Vel);
  attackVel = -m_smoothers.get(C15::Smoothers::Poly_Sync::Env_A_Att_Vel) * _vel;
  decay1Vel = m_smoothers.get(C15::Smoothers::Poly_Sync::Env_A_Dec_1_Vel) * _vel;
  decay2Vel = m_smoothers.get(C15::Smoothers::Poly_Sync::Env_A_Dec_2_Vel) * _vel;
  levelKT = m_smoothers.get(C15::Smoothers::Poly_Sync::Env_A_Lvl_KT) * _pitch;
  if(levelVel < 0.0f)
  {
    peak = std::min(m_convert->eval_level((_vel * levelVel) + levelKT), env_clip_peak);
  }
  else
  {
    peak = std::min(m_convert->eval_level(((1.0f - _vel) * -levelVel) + levelKT), env_clip_peak);
  }
  m_env_a.m_timeFactor[_voiceId][0] = m_convert->eval_level(timeKT + attackVel) * m_millisecond;
  m_env_a.m_timeFactor[_voiceId][1] = m_convert->eval_level(timeKT + decay1Vel) * m_millisecond;
  m_env_a.m_timeFactor[_voiceId][2] = m_convert->eval_level(timeKT + decay2Vel) * m_millisecond;
  m_env_a.setSplitValue(m_smoothers.get(C15::Smoothers::Poly_Fast::Env_A_Elevate));
  m_env_a.setAttackCurve(m_smoothers.get(C15::Smoothers::Poly_Sync::Env_A_Att_Curve));
  m_env_a.setPeakLevel(_voiceId, peak);
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_A_Att) * m_env_a.m_timeFactor[_voiceId][0];
  m_env_a.setSegmentDx(_voiceId, 1, 1.0f / (time + 1.0f));
  m_env_a.setSegmentDest(_voiceId, 1, false, peak);
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_A_Dec_1) * m_env_a.m_timeFactor[_voiceId][1];
  m_env_a.setSegmentDx(_voiceId, 2, 1.0f / (time + 1.0f));
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_A_BP);
  m_env_a.setSegmentDest(_voiceId, 2, true, dest);
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_A_Dec_2) * m_env_a.m_timeFactor[_voiceId][2];
  m_env_a.setSegmentDx(_voiceId, 3, 1.0f / (time + 1.0f));
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_A_Sus);
  m_env_a.setSegmentDest(_voiceId, 3, true, dest);
  // env b
  timeKT = -0.5f * m_smoothers.get(C15::Smoothers::Poly_Sync::Env_B_Time_KT) * _pitch;
  levelVel = m_smoothers.get(C15::Smoothers::Poly_Sync::Env_B_Lvl_Vel);
  attackVel = -m_smoothers.get(C15::Smoothers::Poly_Sync::Env_B_Att_Vel) * _vel;
  decay1Vel = m_smoothers.get(C15::Smoothers::Poly_Sync::Env_B_Dec_1_Vel) * _vel;
  decay2Vel = m_smoothers.get(C15::Smoothers::Poly_Sync::Env_B_Dec_2_Vel) * _vel;
  levelKT = m_smoothers.get(C15::Smoothers::Poly_Sync::Env_B_Lvl_KT) * _pitch;
  if(levelVel < 0.0f)
  {
    peak = std::min(m_convert->eval_level((_vel * levelVel) + levelKT), env_clip_peak);
  }
  else
  {
    peak = std::min(m_convert->eval_level(((1.0f - _vel) * -levelVel) + levelKT), env_clip_peak);
  }
  m_env_b.m_timeFactor[_voiceId][0] = m_convert->eval_level(timeKT + attackVel) * m_millisecond;
  m_env_b.m_timeFactor[_voiceId][1] = m_convert->eval_level(timeKT + decay1Vel) * m_millisecond;
  m_env_b.m_timeFactor[_voiceId][2] = m_convert->eval_level(timeKT + decay2Vel) * m_millisecond;
  m_env_b.setSplitValue(m_smoothers.get(C15::Smoothers::Poly_Fast::Env_B_Elevate));
  m_env_b.setAttackCurve(m_smoothers.get(C15::Smoothers::Poly_Sync::Env_B_Att_Curve));
  m_env_b.setPeakLevel(_voiceId, peak);
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_B_Att) * m_env_b.m_timeFactor[_voiceId][0];
  m_env_b.setSegmentDx(_voiceId, 1, 1.0f / (time + 1.0f));
  m_env_b.setSegmentDest(_voiceId, 1, false, peak);
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_B_Dec_1) * m_env_b.m_timeFactor[_voiceId][1];
  m_env_b.setSegmentDx(_voiceId, 2, 1.0f / (time + 1.0f));
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_B_BP);
  m_env_b.setSegmentDest(_voiceId, 2, true, dest);
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_B_Dec_2) * m_env_b.m_timeFactor[_voiceId][2];
  m_env_b.setSegmentDx(_voiceId, 3, 1.0f / (time + 1.0f));
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_B_Sus);
  m_env_b.setSegmentDest(_voiceId, 3, true, dest);
  // env c
  timeKT = -0.5f * m_smoothers.get(C15::Smoothers::Poly_Sync::Env_C_Time_KT) * _pitch;
  levelVel = m_smoothers.get(C15::Smoothers::Poly_Sync::Env_C_Lvl_Vel);
  attackVel = -m_smoothers.get(C15::Smoothers::Poly_Sync::Env_C_Att_Vel) * _vel;
  levelKT = m_smoothers.get(C15::Smoothers::Poly_Sync::Env_C_Lvl_KT) * _pitch;
  if(levelVel < 0.0f)
  {
    unclipped = m_convert->eval_level((_vel * levelVel) + levelKT);
  }
  else
  {
    unclipped = m_convert->eval_level(((1.0f - _vel) * -levelVel) + levelKT);
  }
  peak = std::min(unclipped, env_clip_peak);
  m_env_c.m_clipFactor[_voiceId] = unclipped / peak;
  m_env_c.m_timeFactor[_voiceId][0] = m_convert->eval_level(timeKT + attackVel) * m_millisecond;
  m_env_c.m_timeFactor[_voiceId][1] = m_convert->eval_level(timeKT + decay1Vel) * m_millisecond;
  m_env_c.m_timeFactor[_voiceId][2] = m_convert->eval_level(timeKT + decay2Vel) * m_millisecond;
  m_env_c.setAttackCurve(m_smoothers.get(C15::Smoothers::Poly_Sync::Env_C_Att_Curve));
  m_env_c.setRetriggerHardness(m_smoothers.get(C15::Smoothers::Poly_Sync::Env_C_Retr_H));
  m_env_c.setPeakLevel(_voiceId, peak);
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_C_Att) * m_env_c.m_timeFactor[_voiceId][0];
  m_env_c.setSegmentDx(_voiceId, 1, 1.0f / (time + 1.0f));
  m_env_c.setSegmentDest(_voiceId, 1, peak);
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_C_Dec_1) * m_env_c.m_timeFactor[_voiceId][1];
  m_env_c.setSegmentDx(_voiceId, 2, 1.0f / (time + 1.0f));
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_C_BP);
  m_env_c.setSegmentDest(_voiceId, 2, dest);
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_C_Dec_2) * m_env_c.m_timeFactor[_voiceId][2];
  m_env_c.setSegmentDx(_voiceId, 3, 1.0f / (time + 1.0f));
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_C_Sus);
  m_env_c.setSegmentDest(_voiceId, 3, dest);
  // start envelopes
  m_env_a.start(_voiceId);
  m_env_b.start(_voiceId);
  m_env_c.start(_voiceId);
  m_env_g.start(_voiceId);
}

void PolySection::stopEnvelopes(const uint32_t _voiceId, const float _pitch, const float _vel)
{
  float time, timeKT, releaseVel;
  // env a
  timeKT = -m_smoothers.get(C15::Smoothers::Poly_Sync::Env_A_Time_KT) * _pitch;
  releaseVel = -m_smoothers.get(C15::Smoothers::Poly_Sync::Env_A_Rel_Vel) * _vel;
  m_env_a.m_timeFactor[_voiceId][3] = m_convert->eval_level(timeKT + releaseVel) * m_millisecond;
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_A_Rel);
  if(time <= env_highest_finite_time)
  {
    time *= m_env_a.m_timeFactor[_voiceId][3];
    m_env_a.setSegmentDx(_voiceId, 4, 1.0f / (time + 1.0f));
  }
  else
  {
    m_env_a.setSegmentDx(_voiceId, 4, 0.0f);
  }
  // env b
  timeKT = -m_smoothers.get(C15::Smoothers::Poly_Sync::Env_B_Time_KT) * _pitch;
  releaseVel = -m_smoothers.get(C15::Smoothers::Poly_Sync::Env_B_Rel_Vel) * _vel;
  m_env_b.m_timeFactor[_voiceId][3] = m_convert->eval_level(timeKT + releaseVel) * m_millisecond;
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_B_Rel);
  if(time <= env_highest_finite_time)
  {
    time *= m_env_b.m_timeFactor[_voiceId][3];
    m_env_b.setSegmentDx(_voiceId, 4, 1.0f / (time + 1.0f));
  }
  else
  {
    m_env_b.setSegmentDx(_voiceId, 4, 0.0f);
  }
  // env c
  timeKT = -m_smoothers.get(C15::Smoothers::Poly_Sync::Env_C_Time_KT) * _pitch;
  releaseVel = -m_smoothers.get(C15::Smoothers::Poly_Sync::Env_C_Rel_Vel) * _vel;
  m_env_c.m_timeFactor[_voiceId][3] = m_convert->eval_level(timeKT + releaseVel) * m_millisecond;
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_C_Rel);
  if(time <= env_highest_finite_time)
  {
    time *= m_env_c.m_timeFactor[_voiceId][3];
    m_env_c.setSegmentDx(_voiceId, 4, 1.0f / (time + 1.0f));
  }
  else
  {
    m_env_c.setSegmentDx(_voiceId, 4, 0.0f);
  }
  // stop envelopes
  m_env_a.stop(_voiceId);
  m_env_b.stop(_voiceId);
  m_env_c.stop(_voiceId);
  m_env_g.stop(_voiceId);
}

void PolySection::updateEnvLevels(const uint32_t _voiceId)
{
  float peak, dest;
  // env a
  m_env_a.setSplitValue(m_smoothers.get(C15::Smoothers::Poly_Fast::Env_A_Elevate));
  peak = m_env_a.m_levelFactor[_voiceId];
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_A_BP);
  m_env_a.setSegmentDest(_voiceId, 2, true, dest);
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_A_Sus);
  m_env_a.setSegmentDest(_voiceId, 3, true, dest);
  // env b
  m_env_b.setSplitValue(m_smoothers.get(C15::Smoothers::Poly_Fast::Env_B_Elevate));
  peak = m_env_b.m_levelFactor[_voiceId];
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_B_BP);
  m_env_b.setSegmentDest(_voiceId, 2, true, dest);
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_B_Sus);
  m_env_b.setSegmentDest(_voiceId, 3, true, dest);
  // env c
  peak = m_env_c.m_levelFactor[_voiceId];
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_C_BP);
  m_env_c.setSegmentDest(_voiceId, 2, dest);
  dest = peak * m_smoothers.get(C15::Smoothers::Poly_Fast::Env_C_Sus);
  m_env_c.setSegmentDest(_voiceId, 3, dest);
}

void PolySection::updateEnvTimes(const uint32_t _voiceId)
{
  float time;
  // env a
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_A_Att) * m_env_a.m_timeFactor[_voiceId][0];
  m_env_a.setSegmentDx(_voiceId, 1, 1.0f / (time + 1.0f));
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_A_Dec_1) * m_env_a.m_timeFactor[_voiceId][1];
  m_env_a.setSegmentDx(_voiceId, 2, 1.0f / (time + 1.0f));
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_A_Dec_2) * m_env_a.m_timeFactor[_voiceId][2];
  m_env_a.setSegmentDx(_voiceId, 3, 1.0f / (time + 1.0f));
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_A_Rel);
  if(time <= env_highest_finite_time)
  {
    time *= m_env_a.m_timeFactor[_voiceId][3];
    m_env_a.setSegmentDx(_voiceId, 4, 1.0f / (time + 1.0f));
  }
  else
  {
    m_env_a.setSegmentDx(_voiceId, 4, 0.0f);
  }
  // env b
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_B_Att) * m_env_b.m_timeFactor[_voiceId][0];
  m_env_b.setSegmentDx(_voiceId, 1, 1.0f / (time + 1.0f));
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_B_Dec_1) * m_env_b.m_timeFactor[_voiceId][1];
  m_env_b.setSegmentDx(_voiceId, 2, 1.0f / (time + 1.0f));
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_B_Dec_2) * m_env_b.m_timeFactor[_voiceId][2];
  m_env_b.setSegmentDx(_voiceId, 3, 1.0f / (time + 1.0f));
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_B_Rel);
  if(time <= env_highest_finite_time)
  {
    time *= m_env_b.m_timeFactor[_voiceId][3];
    m_env_b.setSegmentDx(_voiceId, 4, 1.0f / (time + 1.0f));
  }
  else
  {
    m_env_b.setSegmentDx(_voiceId, 4, 0.0f);
  }
  // env c
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_C_Att) * m_env_c.m_timeFactor[_voiceId][0];
  m_env_c.setSegmentDx(_voiceId, 1, 1.0f / (time + 1.0f));
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_C_Dec_1) * m_env_c.m_timeFactor[_voiceId][1];
  m_env_c.setSegmentDx(_voiceId, 2, 1.0f / (time + 1.0f));
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_C_Dec_2) * m_env_c.m_timeFactor[_voiceId][2];
  m_env_c.setSegmentDx(_voiceId, 3, 1.0f / (time + 1.0f));
  time = m_smoothers.get(C15::Smoothers::Poly_Slow::Env_C_Rel);
  if(time <= env_highest_finite_time)
  {
    time *= m_env_c.m_timeFactor[_voiceId][3];
    m_env_c.setSegmentDx(_voiceId, 4, 1.0f / (time + 1.0f));
  }
  else
  {
    m_env_c.setSegmentDx(_voiceId, 4, 0.0f);
  }
}

void PolySection::updateNotePitch(const uint32_t _voiceId)
{
  const float glide = m_mono_glide.m_value[0];
  m_base_pitch[_voiceId]
      = ((1.0f - glide) * m_last_key_tune[_voiceId]) + (glide * m_key_tune[_voiceId]) + evalScale(_voiceId);
  m_note_pitch[_voiceId] = m_base_pitch[_voiceId]
      + (m_smoothers.get(C15::Smoothers::Poly_Slow::Unison_Detune)
         * m_spread.m_detune[m_uVoice][m_unison_index[_voiceId]])
      + m_smoothers.get(C15::Smoothers::Poly_Slow::Voice_Grp_Tune)
      + m_globalsignals->get(C15::Signals::Global_Signals::Master_Tune);
}

void PolySection::setSlowFilterCoefs(const uint32_t _voiceId)
{
  m_soundgenerator.set(m_signals, _voiceId);
  m_combfilter.set(m_signals, m_samplerate, _voiceId);
  m_feedbackmixer.set(m_signals, _voiceId);
}

void PolySection::testVoiceFadeTable()
{
  nltools::Log::info("VoiceFadeTable:");
  for(int32_t i = 0; i < C15::Config::virtual_key_count; i += 16)
  {
    nltools::Log::info("[", i, "..", i + 15, "]:", m_key_levels[i], "..", m_key_levels[i + 7], "..",
                       m_key_levels[i + 15]);
  }
}
