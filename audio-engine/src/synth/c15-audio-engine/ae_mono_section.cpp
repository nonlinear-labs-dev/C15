#include "ae_mono_section.h"

/******************************************************************************/
/** @file       ae_mono_section.cpp
    @date
    @version    1.7-3
    @author     M. Seeber
    @brief      new container for all monophonic parameters and dsp
    @todo
*******************************************************************************/

MonoSection::MonoSection()
{
}

void MonoSection::init(exponentiator *_convert, LayerSignalCollection *_z_self, const float _ms,
                       const float _samplerate)
{
  // pointer linking
  m_convert = _convert;
  m_z_self = _z_self;
  // init crucial variables (...)
  m_millisecond = _ms;
  m_samplerate = _samplerate;
  m_reciprocal_samplerate = 1.0f / _samplerate;
  m_nyquist = 0.5f * _samplerate;
  // init control shapers
  m_flanger_fb_curve.setCurve(-1.0f, -0.5f, 0.5f, 1.0f);
  m_flanger_rate_to_decay *= NlToolbox::Constants::twopi * m_reciprocal_samplerate;
  m_reverb_color_curve_1.setCurve(66.0f, 137.0f, 130.0f);
  m_reverb_color_curve_2.setCurve(29.0f, 29.0f, 85.0f);
  // init dsp components
  m_flanger.init(_samplerate, 1);  // todo: upsample factor currently not dynamic ...
  m_cabinet.init(_samplerate);
  m_gapfilter.init(_samplerate);
  m_echo.init(_samplerate, 1);    // todo: upsample factor currently not dynamic ...
  m_reverb.init(_samplerate, 1);  // todo: upsample factor currently not dynamic ...
}

void MonoSection::add_copy_sync_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_sync.add_copy_id(_smootherId, _signalId);
}

void MonoSection::add_copy_audio_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_audio.add_copy_id(_smootherId, _signalId);
}

void MonoSection::add_copy_fast_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_fast.add_copy_id(_smootherId, _signalId);
}

void MonoSection::add_copy_slow_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_slow.add_copy_id(_smootherId, _signalId);
}

void MonoSection::start_sync(const uint32_t _id, const float _dest)
{
  m_smoothers.start_sync(_id, _dest);
  if(m_smoothers.m_copy_sync.m_smootherId[_id])
  {
    m_signals.set(m_smoothers.m_copy_sync.m_signalId[_id], _dest);
  }
}

void MonoSection::start_audio(const uint32_t _id, const float _dx, const float _dest)
{
  m_smoothers.start_audio(_id, _dx, _dest);
}

void MonoSection::start_fast(const uint32_t _id, const float _dx, const float _dest)
{
  m_smoothers.start_fast(_id, _dx, _dest);
}

void MonoSection::start_slow(const uint32_t _id, const float _dx, const float _dest)
{
  m_smoothers.start_slow(_id, _dx, _dest);
}

void MonoSection::render_audio(const float _left, const float _right, const float _vol)
{
  m_smoothers.render_audio();
  postProcess_audio();
  // render dsp components (former makemonosound)
  m_flanger.apply(m_signals, _left, _right);
  m_cabinet.apply(m_signals, m_flanger.m_out_L, m_flanger.m_out_R);
  m_gapfilter.apply(m_signals, m_cabinet.m_out_L, m_cabinet.m_out_R);
  m_echo.apply(m_signals, m_gapfilter.m_out_L, m_gapfilter.m_out_R);
  m_reverb.apply(m_signals, m_echo.m_out_L, m_echo.m_out_R);
  // capture z samples
  m_z_self->m_fx_dry = m_reverb.m_out_dry;
  m_z_self->m_fx_wet = m_reverb.m_out_wet;
  // output values
  m_out_l = m_reverb.m_out_L * _vol;
  m_out_r = m_reverb.m_out_R * _vol;
}

void MonoSection::render_fast()
{
  m_smoothers.render_fast();
  postProcess_fast();
  // set fast filter coeffs
  m_flanger.set_fast(m_signals);
}

void MonoSection::render_slow()
{
  m_smoothers.render_slow();
  postProcess_slow();
  // set slow filter coeffs
  m_flanger.set_slow(m_signals);
  m_cabinet.set(m_signals);
  m_gapfilter.set(m_signals);
  m_echo.set(m_signals);
  m_reverb.set(m_signals);
}

void MonoSection::keyDown(PolyKeyEvent *_event)
{
  m_flanger_env.setSegmentDest(0, 1, _event->m_velocity);
  m_flanger_env.start(0);
}

void MonoSection::flushDSP()
{
  m_flanger.resetDSP();
  m_cabinet.resetDSP();
  m_gapfilter.resetDSP();
  m_echo.resetDSP();
  m_reverb.resetDSP();
}

void MonoSection::resetDSP()
{
  flushDSP();
  m_out_l = m_out_r = 0.0f;
}

float MonoSection::evalNyquist(const float _value)
{
  return _value > m_nyquist ? m_nyquist : _value;
}

void MonoSection::postProcess_audio()
{
  auto traversal = &m_smoothers.m_copy_audio;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set(traversal->m_signalId[i], m_smoothers.get_audio(traversal->m_smootherId[i]));
  }
  // flanger
  m_flanger_env.tick(0);
  m_flanger_lfo.tick();
  const float tmp_env = (m_flanger_env.m_body[0].m_signal_magnitude * 2.0f) - 1.0f,
              tmp_wet = m_smoothers.get(C15::Smoothers::Mono_Slow::Flanger_Envelope), tmp_dry = 1.0f - tmp_wet;
  m_signals.set(C15::Signals::Mono_Signals::Flanger_LFO_L, (tmp_dry * m_flanger_lfo.m_left) + (tmp_wet * tmp_env));
  m_signals.set(C15::Signals::Mono_Signals::Flanger_LFO_R, (tmp_dry * m_flanger_lfo.m_right) + (tmp_wet * tmp_env));
}

void MonoSection::postProcess_fast()
{
  auto traversal = &m_smoothers.m_copy_fast;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set(traversal->m_signalId[i], m_smoothers.get_fast(traversal->m_smootherId[i]));
  }
  // temporary variables
  float tmp_val, tmp_dry, tmp_wet, tmp_hi_par, tmp_lo_par, tmp_hi_ser, tmp_lo_ser, tmp_fb;
  // flanger
  m_flanger_lfo.m_phaseOffset = m_flanger_norm_phase * m_smoothers.get(C15::Smoothers::Mono_Slow::Flanger_Phase);
  tmp_fb = m_flanger_fb_curve.applyCurve(m_smoothers.get(C15::Smoothers::Mono_Fast::Flanger_Feedback));
  tmp_val = m_smoothers.get(C15::Smoothers::Mono_Fast::Flanger_Cross_FB);
  m_signals.set(C15::Signals::Mono_Signals::Flanger_FB_Local, tmp_fb * (1.0f - std::abs(tmp_val)));
  m_signals.set(C15::Signals::Mono_Signals::Flanger_FB_Cross, tmp_fb * tmp_val);
  tmp_val = m_smoothers.get(C15::Smoothers::Mono_Fast::Flanger_Mix);
  m_signals.set(C15::Signals::Mono_Signals::Flanger_DRY, 1.0f - std::abs(tmp_val));
  m_signals.set(C15::Signals::Mono_Signals::Flanger_WET, tmp_val);
  tmp_val = m_smoothers.get(C15::Smoothers::Mono_Slow::Flanger_AP_Mod);
  tmp_dry = m_smoothers.get(C15::Smoothers::Mono_Slow::Flanger_AP_Tune);
  m_signals.set(C15::Signals::Mono_Signals::Flanger_APF_L,
                evalNyquist(m_convert->eval_lin_pitch(
                                (m_signals.get(C15::Signals::Mono_Signals::Flanger_LFO_L) * tmp_val) + tmp_dry)
                            * 440.0f));
  m_signals.set(C15::Signals::Mono_Signals::Flanger_APF_R,
                evalNyquist(m_convert->eval_lin_pitch(
                                (m_signals.get(C15::Signals::Mono_Signals::Flanger_LFO_R) * tmp_val) + tmp_dry)
                            * 440.0f));
  // cabinet
  tmp_fb = m_smoothers.get(C15::Smoothers::Mono_Fast::Cabinet_Tilt);
  tmp_val = std::max(m_cabinet_tilt_floor, m_convert->eval_level(0.5f * tmp_fb));
  m_signals.set(C15::Signals::Mono_Signals::Cabinet_Pre_Sat, 0.1588f / tmp_val);
  m_signals.set(C15::Signals::Mono_Signals::Cabinet_Sat, tmp_val);
  m_signals.set(C15::Signals::Mono_Signals::Cabinet_Tilt, tmp_fb);
  tmp_val = m_smoothers.get(C15::Smoothers::Mono_Fast::Cabinet_Mix);
  tmp_wet = m_smoothers.get(C15::Smoothers::Mono_Fast::Cabinet_Cab_Lvl);
  m_signals.set(C15::Signals::Mono_Signals::Cabinet_Dry, 1.0f - tmp_val);
  m_signals.set(C15::Signals::Mono_Signals::Cabinet_Wet, tmp_val * tmp_wet);
  // gap filter
  tmp_val = std::abs(m_smoothers.get(C15::Smoothers::Mono_Fast::Gap_Flt_Mix));
  tmp_wet = NlToolbox::Math::sin(NlToolbox::Constants::halfpi * tmp_val);
  tmp_dry = NlToolbox::Math::sin(NlToolbox::Constants::halfpi * (1.0f - tmp_val));
  tmp_val = m_smoothers.get(C15::Smoothers::Mono_Fast::Gap_Flt_Bal);
  tmp_hi_par = 0.5f + (0.5f * tmp_val);
  tmp_lo_par = 1.f - tmp_hi_par;
  tmp_hi_par = NlToolbox::Math::sin(NlToolbox::Constants::halfpi * tmp_hi_par) * NlToolbox::Constants::sqrt_two;
  tmp_lo_par = NlToolbox::Math::sin(NlToolbox::Constants::halfpi * tmp_lo_par) * NlToolbox::Constants::sqrt_two;
  tmp_val *= tmp_val;
  tmp_hi_ser = m_smoothers.get(C15::Smoothers::Mono_Fast::Gap_Flt_Bal) > 0.0f ? tmp_val : 0.0f;
  tmp_lo_ser = m_smoothers.get(C15::Smoothers::Mono_Fast::Gap_Flt_Bal) > 0.0f ? 0.0f : tmp_val;
  if(m_smoothers.get(C15::Smoothers::Mono_Fast::Gap_Flt_Mix) > 0.0f)
  {
    m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_HP_LP, 0.0f);
    m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_In_LP, 1.0f);
    m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_HP_Out, tmp_wet * tmp_hi_par);
    m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_LP_Out, tmp_wet * tmp_lo_par);
    m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_In_Out, tmp_dry);
  }
  else
  {
    tmp_val = tmp_wet * tmp_hi_ser;
    m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_HP_LP, 1.0f - tmp_lo_ser);
    m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_In_LP, tmp_lo_ser);
    m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_HP_Out,
                  m_signals.get(C15::Signals::Mono_Signals::Gap_Flt_HP_LP) * tmp_val);
    m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_LP_Out, tmp_wet - tmp_val);
    m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_In_Out, (tmp_val * tmp_lo_ser) + tmp_dry);
  }
  // echo
  tmp_fb = m_smoothers.get(C15::Smoothers::Mono_Fast::Echo_Feedback);
  tmp_val = m_smoothers.get(C15::Smoothers::Mono_Fast::Echo_Cross_FB);
  m_signals.set(C15::Signals::Mono_Signals::Echo_FB_Local, tmp_fb * (1.0f - tmp_val));
  m_signals.set(C15::Signals::Mono_Signals::Echo_FB_Cross, tmp_fb * tmp_val);
  tmp_val = m_smoothers.get(C15::Smoothers::Mono_Fast::Echo_Mix);
  m_signals.set(C15::Signals::Mono_Signals::Echo_Wet, (2.0f * tmp_val) - (tmp_val * tmp_val));
  tmp_val = 1.0f - tmp_val;
  m_signals.set(C15::Signals::Mono_Signals::Echo_Dry, (2.0f * tmp_val) - (tmp_val * tmp_val));
  // reverb
  tmp_val = m_smoothers.get(C15::Smoothers::Mono_Fast::Reverb_Mix);
  tmp_dry = 1.0f - tmp_val;
  tmp_dry = (2.0f - tmp_dry) * tmp_dry;
  m_signals.set(C15::Signals::Mono_Signals::Reverb_Dry, tmp_dry);
  tmp_wet = tmp_val;
  tmp_wet = (2.0f - tmp_wet) * tmp_wet;
  m_signals.set(C15::Signals::Mono_Signals::Reverb_Wet, tmp_wet);
}

void MonoSection::postProcess_slow()
{
  auto traversal = &m_smoothers.m_copy_slow;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set(traversal->m_signalId[i], m_smoothers.get_slow(traversal->m_smootherId[i]));
  }
  // temporary variables
  float tmp_Gap, tmp_Center, tmp_Stereo, tmp_Rate, tmp_val, tmp_fb, tmp_dry, tmp_wet;
  // flanger
  tmp_Rate = m_smoothers.get(C15::Smoothers::Mono_Slow::Flanger_Rate);
  m_flanger_lfo.m_increment = m_reciprocal_samplerate * tmp_Rate;
  tmp_Rate *= m_flanger_rate_to_decay;
  m_flanger_env.setSegmentDx(0, 2, tmp_Rate);
  tmp_Center = m_smoothers.get(C15::Smoothers::Mono_Slow::Flanger_Time) * 1.e-3f * m_samplerate;
  tmp_Stereo = m_smoothers.get(C15::Smoothers::Mono_Slow::Flanger_Stereo) * 0.01f;
  m_signals.set(C15::Signals::Mono_Signals::Flanger_Time_L, tmp_Center * (1.0f + tmp_Stereo));
  m_signals.set(C15::Signals::Mono_Signals::Flanger_Time_R, tmp_Center * (1.0f - tmp_Stereo));
  m_signals.set(C15::Signals::Mono_Signals::Flanger_LPF,
                evalNyquist(m_smoothers.get(C15::Smoothers::Mono_Slow::Flanger_Hi_Cut) * 440.0f));
  // cabinet
  m_signals.set(C15::Signals::Mono_Signals::Cabinet_LPF,
                evalNyquist(m_smoothers.get(C15::Smoothers::Mono_Slow::Cabinet_Hi_Cut) * 440.0f));
  m_signals.set(C15::Signals::Mono_Signals::Cabinet_HPF,
                evalNyquist(m_smoothers.get(C15::Smoothers::Mono_Slow::Cabinet_Lo_Cut) * 440.0f));
  // gap filter
  tmp_Gap = (m_smoothers.get(C15::Smoothers::Mono_Fast::Gap_Flt_Mix) < 0.0f ? -1.0f : 1.0f)
      * m_smoothers.get(C15::Smoothers::Mono_Slow::Gap_Flt_Gap);
  tmp_Center = m_smoothers.get(C15::Smoothers::Mono_Slow::Gap_Flt_Center);
  tmp_Stereo = m_smoothers.get(C15::Smoothers::Mono_Slow::Gap_Flt_Stereo);
  m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_LF_L,
                evalNyquist(m_convert->eval_lin_pitch(tmp_Center - tmp_Gap - tmp_Stereo) * 440.0f));
  m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_HF_L,
                evalNyquist(m_convert->eval_lin_pitch(tmp_Center + tmp_Gap - tmp_Stereo) * 440.0f));
  m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_LF_R,
                evalNyquist(m_convert->eval_lin_pitch(tmp_Center - tmp_Gap + tmp_Stereo) * 440.0f));
  m_signals.set(C15::Signals::Mono_Signals::Gap_Flt_HF_R,
                evalNyquist(m_convert->eval_lin_pitch(tmp_Center + tmp_Gap + tmp_Stereo) * 440.0f));
  // echo
  tmp_Center = m_smoothers.get(C15::Smoothers::Mono_Slow::Echo_Time) * m_samplerate;
  tmp_Stereo = m_smoothers.get(C15::Smoothers::Mono_Slow::Echo_Stereo) * m_delay_norm_stereo;
  m_signals.set(C15::Signals::Mono_Signals::Echo_Time_L, tmp_Center * (1.0f + tmp_Stereo));
  m_signals.set(C15::Signals::Mono_Signals::Echo_Time_R, tmp_Center * (1.0f - tmp_Stereo));
  m_signals.set(C15::Signals::Mono_Signals::Echo_LPF,
                evalNyquist(m_smoothers.get(C15::Smoothers::Mono_Slow::Echo_Hi_Cut) * 440.0f));
  // reverb
  tmp_val = m_smoothers.get(C15::Smoothers::Mono_Slow::Reverb_Size);
  tmp_val *= 2.0f - std::abs(tmp_val);
  m_signals.set(C15::Signals::Mono_Signals::Reverb_Size, tmp_val);
  tmp_fb = tmp_val * (0.6f + (0.4f * std::abs(tmp_val)));
  m_signals.set(C15::Signals::Mono_Signals::Reverb_Feedback, 4.32f - (3.32f * tmp_fb));
  tmp_fb = tmp_val * (1.3f - (0.3f * std::abs(tmp_val)));
  m_signals.set(C15::Signals::Mono_Signals::Reverb_Bal, 0.9f * tmp_fb);
  m_signals.set(C15::Signals::Mono_Signals::Reverb_Pre,
                m_smoothers.get(C15::Smoothers::Mono_Slow::Reverb_Pre_Dly) * 200.0f * m_millisecond);
  tmp_val = m_smoothers.get(C15::Smoothers::Mono_Slow::Reverb_Color);
  m_signals.set(C15::Signals::Mono_Signals::Reverb_LPF,
                evalNyquist(m_convert->eval_lin_pitch(m_reverb_color_curve_1.applyCurve(tmp_val)) * 440.0f));
  m_signals.set(C15::Signals::Mono_Signals::Reverb_HPF,
                evalNyquist(m_convert->eval_lin_pitch(m_reverb_color_curve_2.applyCurve(tmp_val)) * 440.0f));
}
