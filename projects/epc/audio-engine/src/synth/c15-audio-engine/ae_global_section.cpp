#include "ae_global_section.h"

/******************************************************************************/
/** @file       ae_global_section.cpp
    @date
    @version    1.7-3
    @author     M. Seeber
    @brief      new container for all global parameters and dsp
    @todo
*******************************************************************************/

GlobalSection::GlobalSection()
{
  m_out_l = m_out_r = 0.0f;
}

void GlobalSection::init(const float _sampleInc)
{
  m_sampleInc = _sampleInc;
}

void GlobalSection::add_copy_sync_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_sync.add_copy_id(_smootherId, _signalId);
}

void GlobalSection::add_copy_audio_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_audio.add_copy_id(_smootherId, _signalId);
}

void GlobalSection::add_copy_fast_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_fast.add_copy_id(_smootherId, _signalId);
}

void GlobalSection::add_copy_slow_id(const uint32_t _smootherId, const uint32_t _signalId)
{
  m_smoothers.m_copy_slow.add_copy_id(_smootherId, _signalId);
}

void GlobalSection::start_sync(const uint32_t _id, const float _dest)
{
  m_smoothers.start_sync(_id, _dest);
  if(m_smoothers.m_copy_sync.m_smootherId[_id])
  {
    m_signals.set(m_smoothers.m_copy_sync.m_signalId[_id], _dest);
  }
}

void GlobalSection::start_audio(const uint32_t _id, const float _dx, const float _dest)
{
  m_smoothers.start_audio(_id, _dx, _dest);
}

void GlobalSection::start_fast(const uint32_t _id, const float _dx, const float _dest)
{
  m_smoothers.start_fast(_id, _dx, _dest);
}

void GlobalSection::start_slow(const uint32_t _id, const float _dx, const float _dest)
{
  m_smoothers.start_slow(_id, _dx, _dest);
}

void GlobalSection::start_base_key(const float _dx, const float _dest)
{
  auto baseKeyValue = m_smoothers.get(C15::Smoothers::Global_Slow::Scale_Base_Key);
  // apply transition and update baseKey-related signals (start, dest, pos)
  const float start
      = m_signals.get(C15::Signals::Global_Signals::Scale_Base_Key_Start) + (m_baseKeyDiff * baseKeyValue);
  m_signals.set(C15::Signals::Global_Signals::Scale_Base_Key_Start, start);
  m_signals.set(C15::Signals::Global_Signals::Scale_Base_Key_Dest, _dest);
  m_baseKeyDiff = _dest - start;

  m_smoothers.sync(C15::Smoothers::Global_Slow::Scale_Base_Key, 0.0f);
  m_smoothers.start(C15::Smoothers::Global_Slow::Scale_Base_Key, _dx, 1.0f);

  m_signals.set(C15::Signals::Global_Signals::Scale_Base_Key_Pos,
                m_smoothers.get(C15::Smoothers::Global_Slow::Scale_Base_Key));
}

void GlobalSection::render_audio(const float _left, const float _right)
{
  // common dsp: smoothers, signal post processing
  m_smoothers.render_audio();
  postProcess_audio();
  // main dsp:
  const float vol = m_smoothers.get(C15::Smoothers::Global_Fast::Master_Volume);
  // (note: master volume seems to not require a signal!)
  // main: test tone
  float phase, squared, signal;
  phase = m_tonePhase - 0.25f;
  phase -= NlToolbox::Conversion::float2int(phase);
  signal = m_toneAmp * NlToolbox::Math::sinP3_noWrap(phase);
  m_tonePhase += m_sampleInc * m_toneFreq;
  m_tonePhase -= NlToolbox::Conversion::float2int(m_tonePhase);
  m_signal[1] = signal;
  // left: volume and test tone combination
  m_signal[0] = vol * _left;
  m_signal[2] = m_signal[0] + m_signal[1];
  signal = m_signal[m_combinationMode];
  if(!APPLY_SOFT_CLIP)
  {
    m_out_l = signal;
  }
  else
  {
    // left: soft clipping
    signal = std::clamp(signal * 0.1588f, -0.25f, 0.25f);
    signal += -0.25f;
    signal += signal;
    signal = 0.5f - std::abs(signal);
    squared = signal * signal;
    m_out_l = signal * ((((2.26548f * squared) - 5.13274f) * squared) + 3.14159f);
  }

  // right: volume and test tone combination
  m_signal[0] = vol * _right;
  m_signal[2] = m_signal[0] + m_signal[1];
  signal = m_signal[m_combinationMode];
  if(!APPLY_SOFT_CLIP)
  {
    m_out_r = signal;
  }
  else
  {
    // right: soft clipping
    signal = std::clamp(signal * 0.1588f, -0.25f, 0.25f);
    signal += -0.25f;
    signal += signal;
    signal = 0.5f - std::abs(signal);
    squared = signal * signal;
    m_out_r = signal * ((((2.26548f * squared) - 5.13274f) * squared) + 3.14159f);
  }
}

void GlobalSection::render_fast()
{
  m_smoothers.render_fast();
  postProcess_fast();
}

void GlobalSection::render_slow()
{
  m_smoothers.render_slow();
  postProcess_slow();
}

void GlobalSection::update_tone_amplitude(const float _db)
{
  m_toneAmp = NlToolbox::Conversion::db2af(_db);
}

void GlobalSection::update_tone_frequency(const float _freq)
{
  m_toneFreq = _freq;
}

void GlobalSection::update_tone_mode(const uint32_t _mode)
{
  m_combinationMode = _mode;
}

void GlobalSection::resetDSP()
{
  m_out_l = m_out_r = 0.0f;
}

void GlobalSection::postProcess_audio()
{
  auto traversal = &m_smoothers.m_copy_audio;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set(traversal->m_signalId[i], m_smoothers.get_audio(traversal->m_smootherId[i]));
  }
}

void GlobalSection::postProcess_fast()
{
  auto traversal = &m_smoothers.m_copy_fast;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set(traversal->m_signalId[i], m_smoothers.get_fast(traversal->m_smootherId[i]));
  }
}

void GlobalSection::postProcess_slow()
{
  auto traversal = &m_smoothers.m_copy_slow;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set(traversal->m_signalId[i], m_smoothers.get_slow(traversal->m_smootherId[i]));
  }
  // apply scale base key fade value (pos)
  m_signals.set(C15::Signals::Global_Signals::Scale_Base_Key_Pos,
                m_smoothers.get(C15::Smoothers::Global_Slow::Scale_Base_Key));
}
