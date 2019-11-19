#include "ae_mono_section.h"

/******************************************************************************/
/** @file       ae_mono_section.cpp
    @date
    @version    1.7-0
    @author     M. Seeber
    @brief      new container for all monophonic parameters and dsp
    @todo
*******************************************************************************/

MonoSection::MonoSection()
{
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

void MonoSection::render_audio(const float _left, const float _right)
{
  m_smoothers.render_audio();
  postProcess_audio();
}

void MonoSection::render_fast()
{
  m_smoothers.render_fast();
  postProcess_fast();
}

void MonoSection::render_slow()
{
  m_smoothers.render_slow();
  postProcess_slow();
}

void MonoSection::keyDown(const float _vel)
{
}

void MonoSection::postProcess_audio()
{
  auto traversal = &m_smoothers.m_copy_audio;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set(traversal->m_signalId[i], m_smoothers.get_audio(traversal->m_smootherId[i]));
  }
}

void MonoSection::postProcess_fast()
{
  auto traversal = &m_smoothers.m_copy_fast;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set(traversal->m_signalId[i], m_smoothers.get_fast(traversal->m_smootherId[i]));
  }
}

void MonoSection::postProcess_slow()
{
  auto traversal = &m_smoothers.m_copy_slow;
  for(uint32_t i = 0; i < traversal->m_length; i++)
  {
    m_signals.set(traversal->m_signalId[i], m_smoothers.get_slow(traversal->m_smootherId[i]));
  }
}
