#include "ae_poly_fb_mix.h"

/******************************************************************************/
/** @file       ae_poly_fb_mix.cpp
    @date
    @version    1.7-3
    @author     A. Schmied, M. Seeber
    @brief      feedback mixer
    @todo
*******************************************************************************/

// todo: FX I/II and Osc Cross handling

Engine::PolyFeedbackMixer::PolyFeedbackMixer()
{
}

void Engine::PolyFeedbackMixer::init(const float _samplerate)
{
  m_warpConst_PI = NlToolbox::Constants::pi / _samplerate;
  m_freqClip_min = _samplerate / 24000.0f;
  m_freqClip_max = _samplerate / 3.0f;
  m_hp30hz_b0 = (NlToolbox::Constants::twopi / _samplerate) * 30.0f;
  m_hp30hz_b0 = std::min(m_hp30hz_b0, 0.8f);
  m_out = m_hp30hz_stateVar = m_hp_b0 = m_hp_b1 = m_hp_a1 = m_hp_stateVar_1 = m_hp_stateVar_2 = 0.0f;
}

void Engine::PolyFeedbackMixer::set(PolySignals &_signals, const uint32_t _voiceId)
{
  auto omega
      = std::clamp(_signals.get(C15::Signals::Truepoly_Signals::FB_Mix_HPF)[_voiceId], m_freqClip_min, m_freqClip_max);
  omega = NlToolbox::Math::tan(omega * m_warpConst_PI);
  float denom = 1.0f / (1.0f + omega);
  m_hp_a1[_voiceId] = (1.0f - omega) * denom;
  m_hp_b0[_voiceId] = denom;
  m_hp_b1[_voiceId] = -denom;
}

void Engine::PolyFeedbackMixer::apply(PolySignals &_signals, const LayerSignalCollection &_self,
                                      const LayerSignalCollection &_other)
{
  // apply part mute of other part (_other.m_mute_factor)
  PolyValue polyFaded, tmpVar = 0.0f;
  float monoFaded, fade;
  // Osc
  fade = _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_Osc_Src);
  polyFaded = ((1.0f - fade) * _other.m_osc_a) + (fade * _other.m_osc_b);
  tmpVar += _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_Osc) * polyFaded;
  // Comb
  fade = _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_Comb_Src);
  polyFaded = ((1.0f - fade) * _self.m_comb) + (fade * _other.m_comb);
  tmpVar += _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_Comb) * polyFaded;
  // SVF
  fade = _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_SVF_Src);
  polyFaded = ((1.0f - fade) * _self.m_svf) + (fade * _other.m_svf);
  tmpVar += _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_SVF) * polyFaded;
  // FX
  fade = _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_Rvb);
  monoFaded = NlToolbox::Crossfades::unipolarCrossFade(_self.m_fx_dry, _self.m_fx_wet, fade);
  tmpVar += (1.0f - _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_FX_Src)) * monoFaded
      * _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_FX);
  monoFaded = NlToolbox::Crossfades::unipolarCrossFade(_other.m_fx_dry, _other.m_fx_wet, fade);
  tmpVar += _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_FX_Src) * monoFaded
      * _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_FX);
  m_out = m_hp_b0 * tmpVar;  // HP
  m_out += m_hp_b1 * m_hp_stateVar_1;
  m_out += m_hp_a1 * m_hp_stateVar_2;
  m_hp_stateVar_1 = tmpVar;
  m_hp_stateVar_2 = m_out;
  m_out *= _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_Drive);
  tmpVar = m_out;
  m_out = sinP3_wrap(m_out);
  m_out = threeRanges(m_out, tmpVar, _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_Fold));
  tmpVar = m_out * m_out;
  tmpVar -= m_hp30hz_stateVar;  // HP 30Hz
  m_hp30hz_stateVar = tmpVar * m_hp30hz_b0 + m_hp30hz_stateVar;
  m_out = parAsym(m_out, tmpVar, _signals.get(C15::Signals::Quasipoly_Signals::FB_Mix_Asym));
  m_out = m_out * _signals.get(C15::Signals::Truepoly_Signals::FB_Mix_Lvl);
}

void Engine::PolyFeedbackMixer::resetDSP()
{
  m_out = m_hp30hz_stateVar = m_hp_stateVar_1 = m_hp_stateVar_2 = 0.0f;
}
