/******************************************************************************/
/** @file           ae_combfilter.cpp
    @date           2018-05-10
    @version        1.0
    @author         Anton Schmied
    @brief          Combfilter
    @todo
*******************************************************************************/

#include "ae_combfilter.h"
#include "SignalStorage.h"

/******************************************************************************/
/** @brief
*******************************************************************************/

ae_combfilter::ae_combfilter()
{
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_combfilter::init(float _samplerate, uint32_t _upsampleFactor)
{
  m_out = 0.f;
  m_decayStateVar = 0.f;

  m_sampleInterval = 1.f / _samplerate;
  m_warpConst_PI = 3.14159f / _samplerate;
  m_warpConst_2PI = 6.28319f / _samplerate;
  m_freqClip_24576 = _samplerate / 24576.f;
  m_freqClip_2 = _samplerate / 2.125f;
  m_freqClip_4 = _samplerate / 4.f;

  //***************************** Highpass *********************************//
  m_hpInStateVar = 0.f;
  m_hpOutStateVar = 0.f;

  //***************************** Lowpass **********************************//
  m_lpStateVar = 0.f;

  //****************************** Allpass *********************************//
  m_apStateVar_1 = 0.f;
  m_apStateVar_2 = 0.f;
  m_apStateVar_3 = 0.f;
  m_apStateVar_4 = 0.f;

  //***************************** Delay ************************************//
  m_buffer_indx = 0;
  m_buffer.resize(COMB_BUFFER_SIZE * _upsampleFactor);
  m_buffer_sz_m1 = COMB_BUFFER_SIZE * _upsampleFactor - 1;
  std::fill(m_buffer.begin(), m_buffer.end(), 0.f);

  m_delayFreqClip = _samplerate / (COMB_BUFFER_SIZE * _upsampleFactor - 2);
  m_delayConst = 0.693147f / (0.0025f * _samplerate);  // 25ms
  m_delaySamples = 0.f;
  m_delayStateVar = 0.f;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_combfilter::setDelaySmoother(uint32_t voice)
{
  m_delayStateVar[voice] = m_delaySamples[voice];
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_combfilter::apply(const FloatVector &_sampleA, const FloatVector &_sampleB, SignalStorage &signals)
{
  //**************************** AB Sample Mix ****************************//
  auto tmpCmbAB = signals.get<Signals::CMB_AB>();  // AB Mix is inverted, so crossfade mix is as well (currently)
  m_out = _sampleB * (1.f - tmpCmbAB) + _sampleA * tmpCmbAB;

  //****************** AB Ssample Phase Mdulation Mix ********************//
  auto tmpCmbPMAB = signals.get<Signals::CMB_PMAB>();
  auto phaseMod = _sampleA * (1.f - tmpCmbPMAB) + _sampleB * tmpCmbPMAB;
  phaseMod *= signals.get<Signals::CMB_PM>();

  //************************** 1-Pole Highpass ****************************//
  auto tmpHP = m_hpCoeff_b0 * m_out;
  tmpHP += (m_hpCoeff_b1 * m_hpInStateVar);
  tmpHP += (m_hpCoeff_a1 * m_hpOutStateVar);

  m_hpInStateVar = m_out + DNC_const;
  m_hpOutStateVar = tmpHP + DNC_const;

  m_out = tmpHP;
  m_out += m_decayStateVar;

  //*************************** 1-Pole Lowpass ****************************//
  m_out *= (1.f - m_lpCoeff);
  m_out += (m_lpCoeff * m_lpStateVar);
  m_out += DNC_const;
  m_lpStateVar = m_out;

  //******************************* Allpass *******************************//
  auto tmpOut = m_out;

  m_out *= m_apCoeff_2;
  m_out += (m_apStateVar_1 * m_apCoeff_1);
  m_out += m_apStateVar_2;

  m_out -= (m_apStateVar_3 * m_apCoeff_1);
  m_out -= (m_apStateVar_4 * m_apCoeff_2);

  m_out += DNC_const;

  m_apStateVar_2 = m_apStateVar_1;
  m_apStateVar_1 = tmpOut;

  m_apStateVar_4 = m_apStateVar_3;
  m_apStateVar_3 = m_out;

  //****************************** Para D ********************************//
  for(uint32_t v = 0; v < dsp_number_of_voices; v++)
  {
    if(std::abs(m_out[v]) > 0.501187f)
    {
      if(m_out[v] > 0.f)
      {
        m_out[v] -= 0.501187f;
        auto tmpVar = m_out[v];

        m_out[v] = std::min(m_out[v], 2.98815f);
        m_out[v] *= (1.f - m_out[v] * 0.167328f);

        m_out[v] *= 0.7488f;
        tmpVar *= 0.2512f;

        m_out[v] += (tmpVar + 0.501187f);
      }
      else
      {
        m_out[v] += 0.501187f;
        auto tmpVar = m_out[v];

        m_out[v] = std::max(m_out[v], -2.98815f);
        m_out[v] *= (1.f - std::abs(m_out[v]) * 0.167328f);

        m_out[v] *= 0.7488f;
        tmpVar *= 0.2512f;

        m_out[v] += (tmpVar - 0.501187f);
      }
    }
  }

  //***************************** SmoothB ********************************//
  auto tmpSmooth = m_delaySamples - m_delayStateVar;
  tmpSmooth *= m_delayConst;
  tmpSmooth += m_delayStateVar;

  m_delayStateVar = tmpSmooth;

  tmpSmooth *= signals.get<Signals::CMB_FEC>();
  tmpSmooth += (phaseMod * tmpSmooth);

  //******************************* Delay ********************************//
  auto holdsample = m_out;  // for Bypass

  for(uint32_t v = 0; v < dsp_number_of_voices; v++)
    m_buffer[m_buffer_indx][v] = m_out[v];

  /// hier kommt voicestealing hin!!

  tmpSmooth -= 1.f;
  tmpSmooth = std::clamp(tmpSmooth, 1.f, 8189.f);

  auto ind_t0 = std::round<int32_t>(tmpSmooth - 0.5f);
  tmpSmooth = tmpSmooth - static_cast<FloatVector>(ind_t0);

  auto ind_tm1 = ind_t0 - 1;
  auto ind_tp1 = ind_t0 + 1;
  auto ind_tp2 = ind_t0 + 2;

  ind_tm1 = m_buffer_indx - ind_tm1;
  ind_t0 = m_buffer_indx - ind_t0;
  ind_tp1 = m_buffer_indx - ind_tp1;
  ind_tp2 = m_buffer_indx - ind_tp2;

  ind_tm1 &= m_buffer_sz_m1;
  ind_t0 &= m_buffer_sz_m1;
  ind_tp1 &= m_buffer_sz_m1;
  ind_tp2 &= m_buffer_sz_m1;

  FloatVector fract, sample_tm1, sample_t0, sample_tp1, sample_tp2;
  fract = tmpSmooth;

  for(uint32_t i = 0; i < dsp_number_of_voices; i++)
  {
    sample_tm1[i] = m_buffer[ind_tm1[i]][i];
    sample_t0[i] = m_buffer[ind_t0[i]][i];
    sample_tp1[i] = m_buffer[ind_tp1[i]][i];
    sample_tp2[i] = m_buffer[ind_tp2[i]][i];
  }
  m_out = interpolRT(tmpSmooth, sample_tm1, sample_t0, sample_tp1, sample_tp2);

  /// Envelope for voicestealingtmpVar

  m_buffer_indx = (m_buffer_indx + 1) & m_buffer_sz_m1;

  auto tmpByp = signals.get<Signals::CMB_BYP>();  // Bypass
  m_out = tmpByp * holdsample + (1.f - tmpByp) * m_out;

  //****************************** Decay ********************************//
  m_decayStateVar = m_out * m_decayGain;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_combfilter::set(SignalStorage &signals, float _samplerate, uint32_t _voiceID)
{
  //********************** Highpass Coefficients *************************//
  float frequency = signals.get<Signals::CMB_FRQ>()[_voiceID];
  frequency *= 0.125f;
  frequency = std::clamp(frequency, m_freqClip_24576, m_freqClip_2);

  frequency *= m_warpConst_PI;
  frequency = NlToolbox::Math::tan(frequency);

  m_hpCoeff_a1[_voiceID] = (1.f - frequency) / (1.f + frequency);
  m_hpCoeff_b0[_voiceID] = 1.f / (1.f + frequency);
  m_hpCoeff_b1[_voiceID] = m_hpCoeff_b0[_voiceID] * -1.f;

  //*********************** Lowpass Coefficient **************************//
  frequency = signals.get<Signals::CMB_LPF>()[_voiceID];
  frequency = std::clamp(frequency, m_freqClip_24576, m_freqClip_4);
  frequency *= m_warpConst_PI;

  frequency *= 0.159155f;  // 2Pi wrap
  frequency -= NlToolbox::Conversion::float2int(frequency);
  frequency *= 6.28319f;

  frequency = NlToolbox::Math::sin(frequency) / NlToolbox::Math::cos(frequency);  // tan -pi..pi

  m_lpCoeff[_voiceID] = (1.f - frequency) / (1.f + frequency);

  //********************** Allpass Coefficients **************************//
  frequency = signals.get<Signals::CMB_APF>()[_voiceID];
  frequency = std::clamp(frequency, m_freqClip_24576, m_freqClip_2);
  frequency *= m_warpConst_2PI;

  float resonance = signals.get<Signals::CMB_APR>() * 1.99f - 1.f;
  resonance = NlToolbox::Math::sin(frequency) * (1.f - resonance);

  float tmpVar = 1.f / (1.f + resonance);

  m_apCoeff_1[_voiceID] = (-2.f * NlToolbox::Math::cos(frequency)) * tmpVar;
  m_apCoeff_2[_voiceID] = (1.f - resonance) * tmpVar;

  //*************************** Delaytime ********************************//
  frequency = signals.get<Signals::CMB_FRQ>()[_voiceID];

  if(frequency < m_delayFreqClip)
  {
    m_delaySamples[_voiceID] = _samplerate / m_delayFreqClip;
  }
  else
  {
    m_delaySamples[_voiceID] = _samplerate / frequency;
  }

  //************************ Lowpass Influence ***************************//
  frequency *= m_sampleInterval;

  float stateVar_r = NlToolbox::Math::sinP3_wrap(frequency);
  float stateVar_i = NlToolbox::Math::sinP3_wrap(frequency + 0.25f);

  stateVar_r = stateVar_r * m_lpCoeff[_voiceID];
  stateVar_i = stateVar_i * -m_lpCoeff[_voiceID] + 1.f;

  tmpVar = NlToolbox::Math::arctan(stateVar_r / stateVar_i) * -0.159155f;  // (1.f / -6.28318f)

  m_delaySamples[_voiceID] = m_delaySamples[_voiceID] * tmpVar + m_delaySamples[_voiceID];

  //************************ Allpass Influence ***************************//
  stateVar_i = NlToolbox::Math::sinP3_wrap(frequency) * -1.f * m_apCoeff_1[_voiceID];
  stateVar_r = NlToolbox::Math::sinP3_wrap(frequency + 0.25f) * m_apCoeff_1[_voiceID];

  float stateVar2_i = NlToolbox::Math::sinP3_wrap(frequency + frequency);
  float stateVar2_r = NlToolbox::Math::sinP3_wrap(frequency + frequency + 0.25f);

  float var1_i = stateVar_i - stateVar2_i;
  float var2_i = (stateVar_i - (m_apCoeff_2[_voiceID] * stateVar2_i)) * -1.f;
  float var1_r = stateVar_r + stateVar2_r + m_apCoeff_2[_voiceID];
  float var2_r = stateVar_r + (stateVar2_r * m_apCoeff_2[_voiceID]) + 1.f;

  stateVar_r = (var1_r * var2_r) - (var1_i * var2_i);
  stateVar_i = (var1_r * var2_i) + (var2_r * var1_i);

  if(stateVar_r > 0.f)  // safe
  {
    stateVar_r += 1e-12f;
  }
  else
  {
    stateVar_r -= 1e-12f;
  }

  tmpVar = NlToolbox::Math::arctan(stateVar_i / stateVar_r);  // arctan

  if(stateVar_r < 0.f)
  {
    if(stateVar_i > 0.f)
    {
      tmpVar += 3.14159f;
    }
    else
    {
      tmpVar -= 3.14159f;
    }
  }

  if(tmpVar > 0.f)  // forced unwrap > 0
  {
    tmpVar -= 6.28318f;
  }

  tmpVar *= 0.159155f;

  m_delaySamples[_voiceID] = m_delaySamples[_voiceID] * tmpVar + m_delaySamples[_voiceID];

  //**************************** Decay Gain ******************************//
  tmpVar = signals.get<Signals::CMB_DEC>()[_voiceID];
  frequency = signals.get<Signals::CMB_FRQ>()[_voiceID] * std::abs(tmpVar);

  frequency = std::max(frequency, DNC_const);
  frequency = (1.f / frequency) * -6.28318f;

  if(frequency > 0)  // Exp Clipped
  {
    frequency = 1.f;
  }
  else if(-27.631f > frequency)
  {
    frequency = 0.f;
  }
  else
  {
    frequency = std::pow(2.71828f, frequency);
  }

  if(tmpVar < 0.f)
  {
    frequency *= -1.f;
  }

  m_decayGain[_voiceID] = frequency;
}

/******************************************************************************/
/** @brief
*******************************************************************************/

void ae_combfilter::resetDSP()
{
  m_out = 0.f;
  m_decayStateVar = 0.f;

  //***************************** Highpass *********************************//
  m_hpInStateVar = 0.f;
  m_hpOutStateVar = 0.f;

  //***************************** Lowpass **********************************//
  m_lpStateVar = 0.f;

  //****************************** Allpass *********************************//
  m_apStateVar_1 = 0.f;
  m_apStateVar_2 = 0.f;
  m_apStateVar_3 = 0.f;
  m_apStateVar_4 = 0.f;

  //***************************** Delay ************************************//
  std::fill(m_buffer.begin(), m_buffer.end(), 0.f);

  m_delaySamples = 0.f;
  m_delayStateVar = 0.f;
}
