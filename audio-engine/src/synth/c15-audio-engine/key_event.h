#pragma once

/******************************************************************************/
/** @file       key_event.h
    @date
    @version    1.7-3
    @author     M. Seeber
    @brief      primitives: key event data structure
                (index, active, steal, unison, position, velocity, tune, trigger)
    @todo
*******************************************************************************/

#include "parameter-db/generated/c15_config.h"

struct PolyKeyEvent
{
  uint32_t m_localIndex = 0, m_voiceId = 0, m_unisonIndex = 0, m_position = 0;
  float m_velocity = 0.0f, m_tune = 0.0f;
  bool m_active = false, m_stolen = false, m_trigger_env = false, m_trigger_glide = false, m_trigger_phase = false;
};

template <uint32_t Voices> class PolyKeyPacket
{
 public:
  inline PolyKeyPacket()
  {
  }
  inline void init()
  {
    m_length = 0;
  }
  inline void startEvent(const uint32_t _keyPos, const float _vel, const bool _retrigger_env,
                         const bool _retrigger_glide, const bool _retrigger_phase)
  {
    // setting up common settings
    m_data[m_length].m_position = _keyPos;
    m_data[m_length].m_velocity = _vel;
    m_data[m_length].m_tune = static_cast<float>(_keyPos) - static_cast<float>(C15::Config::key_center);
    m_data[m_length].m_trigger_env = _retrigger_env;
    m_data[m_length].m_trigger_glide = _retrigger_glide;
    m_data[m_length].m_trigger_phase = _retrigger_phase;
  }
  inline PolyKeyEvent* first()
  {
    reset();
    return &m_data[m_index];
  }
  inline bool running()
  {
    return m_index < m_length;
  }
  inline PolyKeyEvent* next()
  {
    m_index++;
    return &m_data[m_index];
  }
  inline void reset()
  {
    m_index = 0;
  }
  inline void add(const uint32_t _localIndex, const uint32_t _voiceId, const uint32_t _unisonIndex, const bool _active,
                  const bool _stolen)
  {
    // adding unison info to common settings
    m_data[m_length].m_localIndex = _localIndex;
    m_data[m_length].m_voiceId = _voiceId;
    m_data[m_length].m_unisonIndex = _unisonIndex;
    m_data[m_length].m_active = _active;
    m_data[m_length].m_stolen = _stolen;
    // common settings are propagated
    propagateEvent();
  }

 private:
  inline void propagateEvent()
  {
    m_data[m_length].m_position = m_data[0].m_position;
    m_data[m_length].m_velocity = m_data[0].m_velocity;
    m_data[m_length].m_tune = m_data[0].m_tune;
    m_data[m_length].m_trigger_env = m_data[0].m_trigger_env;
    m_data[m_length].m_trigger_phase = m_data[0].m_trigger_phase;
    m_length++;
  }
  PolyKeyEvent m_data[Voices];
  uint32_t m_index = 0, m_length = 0;
};
