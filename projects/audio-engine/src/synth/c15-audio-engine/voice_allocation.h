#pragma once

/******************************************************************************/
/** @file       voice_allocation.h
    @date
    @version    1.7-4
    @author     M. Seeber
    @brief      based on former LPC implementation (nl_tcd_valloc.c)
                - two abstractional layers facilitating different tasks:
                  1) Allocator: basic Voice assignment algorithm
                  2) Allocation: providing global and local allocators, perform
                  Unison, Steal, Split etc.
                - safe against invalid arguments
                - providing Poly and Mono modes
                - compatible with Single/Split/Layer modes
    @todo
*******************************************************************************/

#include "../../parameter-db/generated/parameter_info.h"
#include "key_event.h"
#include "mappable_list.h"

// Descriptors

using LayerMode = C15::Properties::LayerMode;

enum class AllocatorId
{
  None,
  Global,
  Local_I,
  Local_II,
  Dual
};

enum class MonoPriority
{
  Lowest,
  Latest,
  Highest
};

// Primitive Data Structures

struct KeyAssignment
{
  AllocatorId m_origin = AllocatorId::None;
  uint32_t m_voiceId = 0, m_position = 0, m_key = 0;
  float m_velocity = 0.0f;
  bool m_active = false;
  inline void setEvent(const uint32_t _keyPos, const float _vel)
  {
    m_position = m_key = _keyPos;
    m_velocity = _vel;
  }
  inline uint32_t setVoiceId(const uint32_t _voiceId, const uint32_t _unison)
  {
    m_voiceId = _voiceId;
    return _unison * _voiceId;
  }
};

struct VoiceAssignment
{
  uint32_t m_keyId = 0;
  bool m_active = false, m_stolen = false;
};

// Basic Allocators

// Mono Voice Allocator
template <uint32_t Keys> class MonoVoiceAllocator
{
 public:
  // public member variables provide key event information and allocator settings
  MonoPriority m_priority = MonoPriority::Latest;
  uint32_t m_key_position = 0;
  bool m_enabled = false, m_legato_on_env = false, m_legato_on_glide = false, m_suppress_first_glide = false,
       m_retrigger_env = false, m_retrigger_glide = false;
  inline MonoVoiceAllocator()
  {
  }
  inline void keyDown(const uint32_t _keyPosition)
  {
    // prior states
    const bool priorKeysPressed = (m_latest.m_assigned > 0);
    // update sorted lists
    m_latest.appendElement(_keyPosition);
    m_highest.insertElement(_keyPosition);
    // resolve priority and legato conditions
    switch(m_priority)
    {
      case MonoPriority::Lowest:
        // if priority is lowest:
        m_key_position = m_highest.getFirstElement();
        if(m_highest.isFirstElement(_keyPosition))
        {
          // if key is lowest:
          m_retrigger_env = priorKeysPressed ? (!m_legato_on_env) : true;
          m_retrigger_glide = m_suppress_first_glide ? false : (m_legato_on_glide ? priorKeysPressed : true);
        }
        else
        {
          // if key is not lowest:
          m_retrigger_env = m_retrigger_glide = false;
        }
        break;
      case MonoPriority::Latest:
        // if priority is latest: (key is always latest)
        m_key_position = _keyPosition;
        m_retrigger_env = priorKeysPressed ? (!m_legato_on_env) : true;
        m_retrigger_glide = m_suppress_first_glide ? false : (m_legato_on_glide ? priorKeysPressed : true);
        break;
      case MonoPriority::Highest:
        // if priority is highest:
        m_key_position = m_highest.getLastElement();
        if(m_highest.isLastElement(_keyPosition))
        {
          // if key is highest:
          m_retrigger_env = priorKeysPressed ? (!m_legato_on_env) : true;
          m_retrigger_glide = m_suppress_first_glide ? false : (m_legato_on_glide ? priorKeysPressed : true);
        }
        else
        {
          // if key is not lowest:
          m_retrigger_env = m_retrigger_glide = false;
        }
        break;
    }
    // clear first glide suppression
    if(m_suppress_first_glide)
    {
      m_suppress_first_glide = false;
    }
  }
  inline void keyUp(const uint32_t _keyPosition)
  {
    // update sorted lists
    m_latest.removeElement(_keyPosition);
    m_highest.removeElement(_keyPosition);
    // subsequent states
    const bool stillKeysPressed = (m_latest.m_assigned > 0);
    // trigger conditions
    m_retrigger_env = !stillKeysPressed;
    m_retrigger_glide = m_legato_on_glide ? stillKeysPressed : true;
    // resolve priority and legato conditions
    switch(m_priority)
    {
      case MonoPriority::Lowest:
        m_key_position = stillKeysPressed ? m_highest.getFirstElement() : _keyPosition;
        break;
      case MonoPriority::Latest:
        m_key_position = stillKeysPressed ? m_latest.getLastElement() : _keyPosition;
        break;
      case MonoPriority::Highest:
        m_key_position = stillKeysPressed ? m_highest.getLastElement() : _keyPosition;
        break;
    }
  }
  inline void reset()
  {
    m_latest.reset();
    m_highest.reset();
  }

 private:
  SortedList<Keys> m_latest, m_highest;
};

// Poly Voice Allocator
template <uint32_t Voices> class PolyVoiceAllocator
{
 public:
  inline PolyVoiceAllocator()
  {
    m_unison = 1;
    init(Voices);
  }
  // keyDown algorithm
  inline uint32_t keyDown()
  {
    uint32_t voiceId;
    if(m_num_assigned < m_assignable)
    {
      if(m_num_assigned == 0)
      {
        m_oldest_assigned = m_youngest_assigned = m_oldest_released;
      }
      voiceId = m_oldest_released;
      m_oldest_released = m_next_released[voiceId];
      m_num_assigned++;
    }
    else
    {
      voiceId = m_oldest_assigned;
      m_oldest_assigned = m_next_assigned[voiceId];
    }
    m_next_assigned[m_youngest_assigned] = voiceId;
    m_previous_assigned[voiceId] = m_youngest_assigned;
    m_youngest_assigned = voiceId;
    return voiceId;
  }
  // keyUp algorithm
  inline uint32_t keyUp(const uint32_t _voiceId)
  {
    m_next_released[m_youngest_released] = _voiceId;
    m_youngest_released = _voiceId;
    if(m_num_assigned == m_assignable)
    {
      m_oldest_released = _voiceId;
    }
    m_num_assigned--;
    if(m_oldest_assigned == _voiceId)
    {
      m_oldest_assigned = m_next_assigned[_voiceId];
    }
    else if(m_youngest_assigned == _voiceId)
    {
      m_youngest_assigned = m_previous_assigned[_voiceId];
    }
    else
    {
      m_next_assigned[m_previous_assigned[_voiceId]] = m_next_assigned[_voiceId];
      m_previous_assigned[m_next_assigned[_voiceId]] = m_previous_assigned[_voiceId];
    }
    return _voiceId;
  }
  // unison setter
  inline void setUnison(const uint32_t _voices)
  {
    m_unison = _voices;
    init(Voices / _voices);
  }
  // (internal) initialization
  inline void init(const uint32_t _assignable)
  {
    m_assignable = _assignable;
    m_num_assigned = m_oldest_assigned = m_youngest_assigned = m_oldest_released = 0;
    m_youngest_released = m_assignable - 1;
    for(uint32_t i = 0; i < m_assignable - 1; i++)
    {
      m_next_released[i] = i + 1;
    }
  }
  // (internal) unison getter
  inline uint32_t getUnison()
  {
    return m_unison;
  }

 private:
  uint32_t m_previous_assigned[Voices] = {}, m_next_assigned[Voices] = {}, m_next_released[Voices] = {},
           m_assignable = {}, m_num_assigned = {}, m_oldest_assigned = {}, m_youngest_assigned = {},
           m_oldest_released = {}, m_youngest_released = {}, m_unison = {};
  // m_mono_keys[Voices] = {}, m_mono_latest = {}
};

// Main Voice Allocation Structure

template <uint32_t GlobalVoices, uint32_t LocalVoices, uint32_t Keys> class VoiceAllocation
{
 public:
  PolyKeyPacket<GlobalVoices> m_traversal;
  uint32_t m_unison = {}, m_splitPoint = {};
  inline VoiceAllocation()
  {
  }
  inline void init()
  {
    // prepare lookup tables for local index (I, II) and voices (0 ... LocalVoices -1)
    for(uint32_t v = 0; v < GlobalVoices; v++)
    {
      m_localIndex[v] = v / LocalVoices;
      m_localVoice[v] = v % LocalVoices;
      m_glideAllowance[v] = v == LocalVoices;
    }
  }
  inline bool keyDown(const uint32_t _keyPos, const float _vel, LayerMode currentMode)
  {
    // validation 1 - keyPos_in_range ?
    bool validity = _keyPos < Keys;
    if(validity)
    {
      KeyAssignment* keyState = &m_keyState[_keyPos];
      // validation 2 - key_released ?
      validity = !keyState->m_active;
      if(validity)
      {
        keyState->setEvent(_keyPos, _vel);
        keyDown_apply(keyState, currentMode);
        keyDown_confirm(keyState);
      }
    }
    return validity;
  }
  inline bool keyUp(const uint32_t _keyPos, const float _vel)
  {
    // validation 1 - keyPos_in_range ?
    bool validity = _keyPos < Keys;
    if(validity)
    {
      KeyAssignment* keyState = &m_keyState[_keyPos];
      // validation 2 - key_pressed ?
      validity = keyState->m_active;
      if(validity)
      {
        keyState->setEvent(_keyPos, _vel);
        validity = keyUp_apply(keyState);
      }
      keyUp_confirm(keyState);
    }
    return validity;
  }
  inline bool setSplitPoint(const uint32_t _keyPos)
  {
    // validation - keyPos_in_range ?
    bool validity = _keyPos < Keys;
    if(validity)
    {
      m_splitPoint = _keyPos;
    }
    return validity;
  }
  inline bool setUnison(const uint32_t _layer, const float _value, LayerMode oldMode, LayerMode newMode)
  {
    uint32_t voices = 1;
    bool validity = false;
    switch(oldMode)
    {
      case LayerMode::Single:
        clear_keyState(AllocatorId::Global);
        break;
      case LayerMode::Split:
        clear_keyState(m_layerId[_layer]);
        break;
      case LayerMode::Layer:
        clear_keyState(AllocatorId::Dual);
        break;
    }

    switch(newMode)
    {
      case LayerMode::Single:
        voices += static_cast<uint32_t>((_value + 0.01f) * (static_cast<float>(GlobalVoices - 1)));
        m_global.setUnison(voices);
        validity = true;
        break;
      case LayerMode::Split:
        voices += static_cast<uint32_t>(_value * (static_cast<float>(LocalVoices - 1)));
        m_local[_layer].setUnison(voices);
        validity = true;
        break;
      case LayerMode::Layer:
        voices += static_cast<uint32_t>(_value * (static_cast<float>(LocalVoices - 1)));
        m_local[0].setUnison(voices);
        validity = true;
        break;
    }
    m_unison = voices;
    return validity;
  }
  inline void setMonoEnable(const uint32_t _layerId, const float _value, LayerMode currentMode)
  {
    const bool mono = static_cast<bool>(_value);
    switch(currentMode)
    {
      case LayerMode::Single:
        clear_keyState(AllocatorId::Global);
        m_global_mono.reset();
        m_global_mono.m_enabled = m_global_mono.m_suppress_first_glide = mono;
        break;
      case LayerMode::Split:
        clear_keyState(m_layerId[_layerId]);
        m_local_mono[_layerId].reset();
        m_local_mono[_layerId].m_enabled = m_local_mono[_layerId].m_suppress_first_glide = mono;
        break;
      case LayerMode::Layer:
        clear_keyState(AllocatorId::Dual);
        m_local_mono[0].reset();
        m_local_mono[0].m_enabled = m_local_mono[0].m_suppress_first_glide = mono;
        break;
    }
  }
  inline void setMonoPriority(const uint32_t _layerId, const float _value, LayerMode currentMode)
  {
    const MonoPriority prio = static_cast<MonoPriority>(_value);
    switch(currentMode)
    {
      case LayerMode::Single:
        m_global_mono.m_priority = prio;
        break;
      case LayerMode::Split:
        m_local_mono[_layerId].m_priority = prio;
        break;
      case LayerMode::Layer:
        m_local_mono[0].m_priority = prio;
        break;
    }
  }
  inline void setMonoLegato(const uint32_t _layerId, const float _value, LayerMode currentMode)
  {
    const uint32_t mode = static_cast<uint32_t>(_value);
    switch(currentMode)
    {
      case LayerMode::Single:
        m_global_mono.m_legato_on_env = mode & 1;
        m_global_mono.m_legato_on_glide = mode & 2;
        break;
      case LayerMode::Split:
        m_local_mono[_layerId].m_legato_on_env = mode & 1;
        m_local_mono[_layerId].m_legato_on_glide = mode & 2;
        break;
      case LayerMode::Layer:
        m_local_mono[0].m_legato_on_env = mode & 1;
        m_local_mono[0].m_legato_on_glide = mode & 2;
        break;
    }
  }
  // resets everything
  inline void reset()
  {
    clear_keyState();
  }

 private:
  PolyVoiceAllocator<GlobalVoices> m_global;
  PolyVoiceAllocator<LocalVoices> m_local[2];
  MonoVoiceAllocator<Keys> m_global_mono;
  MonoVoiceAllocator<Keys> m_local_mono[2];
  KeyAssignment m_keyState[Keys];
  VoiceAssignment m_voiceState[GlobalVoices];
  uint32_t m_localIndex[GlobalVoices] = {}, m_localVoice[GlobalVoices] = {};
  bool m_glideAllowance[GlobalVoices] = {};
  const AllocatorId m_layerId[2] = { AllocatorId::Local_I, AllocatorId::Local_II };
  inline void keyDown_apply(KeyAssignment* _keyState, LayerMode currentMode)
  {
    m_traversal.init();
    uint32_t firstVoice, unisonVoices;
    // determine associated allocator (by mode)
    switch(currentMode)
    {
      case LayerMode::Single:
        _keyState->m_origin = AllocatorId::Global;
        unisonVoices = m_global.getUnison();
        if(m_global_mono.m_enabled)
        {
          // single mono keyDown
          m_global_mono.keyDown(_keyState->m_key);
          firstVoice = _keyState->setVoiceId(0, unisonVoices);
          _keyState->m_position = m_global_mono.m_key_position;
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, m_global_mono.m_retrigger_env,
                                 m_global_mono.m_retrigger_glide);
        }
        else
        {
          // single poly keyDown
          firstVoice = _keyState->setVoiceId(m_global.keyDown(), unisonVoices);
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, true, false);
          //clear stolen key first(all associated voices will be lost)
          if(m_voiceState[firstVoice].m_active)
          {
            keyUp_confirm(&m_keyState[m_voiceState[firstVoice].m_keyId]);
          }
        }
        // unison loop
        keyDown_unisonLoop(_keyState->m_position, firstVoice, unisonVoices);
        break;
      case LayerMode::Split:
        // determine split target
        if(_keyState->m_position <= m_splitPoint)
        {
          _keyState->m_origin = AllocatorId::Local_I;
          unisonVoices = m_local[0].getUnison();
          if(m_local_mono[0].m_enabled)
          {
            // split[I] mono keyDown
            m_local_mono[0].keyDown(_keyState->m_key);
            firstVoice = _keyState->setVoiceId(0, unisonVoices);
            _keyState->m_position = m_local_mono[0].m_key_position;
            m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, m_local_mono[0].m_retrigger_env,
                                   m_local_mono[0].m_retrigger_glide);
          }
          else
          {
            // split[I] poly keyDown
            firstVoice = _keyState->setVoiceId(m_local[0].keyDown(), unisonVoices);
            m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, true, false);
            // clear stolen key first (all associated voices will be lost)
            if(m_voiceState[firstVoice].m_active)
            {
              keyUp_confirm(&m_keyState[m_voiceState[firstVoice].m_keyId]);
            }
          }
          // unison loop
          keyDown_unisonLoop(_keyState->m_position, firstVoice, unisonVoices);
        }
        else
        {
          _keyState->m_origin = AllocatorId::Local_II;
          unisonVoices = m_local[1].getUnison();
          if(m_local_mono[1].m_enabled)
          {
            // split[II] mono keyDown
            m_local_mono[1].keyDown(_keyState->m_key);
            firstVoice = LocalVoices + _keyState->setVoiceId(0, unisonVoices);
            _keyState->m_position = m_local_mono[1].m_key_position;
            m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, m_local_mono[1].m_retrigger_env,
                                   m_local_mono[1].m_retrigger_glide);
          }
          else
          {
            // split[II] poly keyDown
            firstVoice = LocalVoices + _keyState->setVoiceId(m_local[1].keyDown(), unisonVoices);
            m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, true, false);
            // clear stolen key first (all associated voices will be lost)
            if(m_voiceState[firstVoice].m_active)
            {
              keyUp_confirm(&m_keyState[m_voiceState[firstVoice].m_keyId]);
            }
          }
          // unison loop
          keyDown_unisonLoop(_keyState->m_position, firstVoice, unisonVoices);
        }
        break;
      case LayerMode::Layer:
        _keyState->m_origin = AllocatorId::Dual;
        unisonVoices = m_local[0].getUnison();
        if(m_local_mono[0].m_enabled)
        {
          // layer[I&II] mono keyDown
          m_local_mono[0].keyDown(_keyState->m_key);
          firstVoice = _keyState->setVoiceId(0, unisonVoices);
          _keyState->m_position = m_local_mono[0].m_key_position;
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, m_local_mono[0].m_retrigger_env,
                                 m_local_mono[0].m_retrigger_glide);
        }
        else
        {
          // layer[I&II] poly keyDown
          firstVoice = _keyState->setVoiceId(m_local[0].keyDown(), unisonVoices);
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, true, false);
          // clear stolen key first (all associated voices will be lost)
          if(m_voiceState[firstVoice].m_active)
          {
            keyUp_confirm(&m_keyState[m_voiceState[firstVoice].m_keyId]);
          }
        }
        // unison loop
        keyDown_unisonLoop(_keyState->m_position, firstVoice, unisonVoices);
        keyDown_unisonLoop(_keyState->m_position, LocalVoices + firstVoice, unisonVoices);
        break;
    }
  }
  inline bool keyUp_apply(KeyAssignment* _keyState)
  {
    m_traversal.init();
    uint32_t firstVoice, unisonVoices;
    // determine associated allocator (by origin)
    switch(_keyState->m_origin)
    {
      case AllocatorId::None:
        return false;
      case AllocatorId::Global:
        unisonVoices = m_global.getUnison();
        if(m_global_mono.m_enabled)
        {
          // single mono keyUp
          m_global_mono.keyUp(_keyState->m_position);
          _keyState->m_voiceId = 0;
          _keyState->m_position = m_global_mono.m_key_position;
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, m_global_mono.m_retrigger_env,
                                 m_global_mono.m_retrigger_glide);
        }
        else
        {
          // single poly keyUp
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, true, false);
          m_global.keyUp(_keyState->m_voiceId);
        }
        firstVoice = _keyState->m_voiceId * unisonVoices;
        // unison loop
        keyUp_unisonLoop(firstVoice, unisonVoices);
        break;
      case AllocatorId::Local_I:
        unisonVoices = m_local[0].getUnison();
        if(m_local_mono[0].m_enabled)
        {
          // split[I] mono keyUp
          m_local_mono[0].keyUp(_keyState->m_position);
          _keyState->m_voiceId = 0;
          _keyState->m_position = m_local_mono[0].m_key_position;
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, m_local_mono[0].m_retrigger_env,
                                 m_local_mono[0].m_retrigger_glide);
        }
        else
        {
          // split[I] poly keyUp
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, true, false);
          m_local[0].keyUp(_keyState->m_voiceId);
        }
        firstVoice = _keyState->m_voiceId * unisonVoices;
        // unison loop
        keyUp_unisonLoop(firstVoice, unisonVoices);
        break;
      case AllocatorId::Local_II:
        unisonVoices = m_local[1].getUnison();
        if(m_local_mono[1].m_enabled)
        {
          // split[II] mono keyUp
          m_local_mono[1].keyUp(_keyState->m_position);
          _keyState->m_voiceId = 0;
          _keyState->m_position = m_local_mono[1].m_key_position;
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, m_local_mono[1].m_retrigger_env,
                                 m_local_mono[1].m_retrigger_glide);
        }
        else
        {
          // split[II] poly keyUp
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, true, false);
          m_local[1].keyUp(_keyState->m_voiceId);
        }
        firstVoice = _keyState->m_voiceId * unisonVoices;
        // unison loop
        keyUp_unisonLoop(LocalVoices + firstVoice, unisonVoices);
        break;
      case AllocatorId::Dual:
        unisonVoices = m_local[0].getUnison();
        if(m_local_mono[0].m_enabled)
        {
          // layer[I&II] mono keyUp
          m_local_mono[0].keyUp(_keyState->m_position);
          _keyState->m_voiceId = 0;
          _keyState->m_position = m_local_mono[0].m_key_position;
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, m_local_mono[0].m_retrigger_env,
                                 m_local_mono[0].m_retrigger_glide);
        }
        else
        {
          // layer[I&II] poly keyUp
          m_traversal.startEvent(_keyState->m_position, _keyState->m_velocity, true, false);
          m_local[0].keyUp(_keyState->m_voiceId);
        }
        firstVoice = _keyState->m_voiceId * unisonVoices;
        // unison loop
        keyUp_unisonLoop(firstVoice, unisonVoices);
        keyUp_unisonLoop(LocalVoices + firstVoice, unisonVoices);
        break;
    }
    return true;
  }
  inline void keyDown_confirm(KeyAssignment* _keyState)
  {
    _keyState->m_active = true;
  }
  inline void keyUp_confirm(KeyAssignment* _keyState)
  {
    _keyState->m_origin = AllocatorId::None;
    _keyState->m_active = false;
  }
  inline void keyDown_unisonLoop(const uint32_t _keyId, const uint32_t _firstVoice, const uint32_t _unisonVoices)
  {
    for(uint32_t unisonIndex = 0; unisonIndex < _unisonVoices; unisonIndex++)
    {
      const uint32_t voiceId = _firstVoice + unisonIndex;
      // glide is allowed for: first unison voice and: first unison voice of second voice group (important for single-sound cases)
      const bool allow_glide = unisonIndex == 0 ? true : m_glideAllowance[voiceId];
      VoiceAssignment* voiceState = &m_voiceState[voiceId];
      voiceState->m_keyId = _keyId;
      voiceState->m_stolen = voiceState->m_active;
      voiceState->m_active = true;
      m_traversal.add(m_localIndex[voiceId], m_localVoice[voiceId], unisonIndex, voiceState->m_active,
                      voiceState->m_stolen, allow_glide);
    }
  }
  inline void keyUp_unisonLoop(const uint32_t _firstVoice, const uint32_t _unisonVoices)
  {
    for(uint32_t unisonIndex = 0; unisonIndex < _unisonVoices; unisonIndex++)
    {
      const uint32_t voiceId = _firstVoice + unisonIndex;
      // glide is allowed for: first unison voice and: first unison voice of second voice group (important for single-sound cases)
      const bool allow_glide = unisonIndex == 0 ? true : m_glideAllowance[voiceId];
      VoiceAssignment* voiceState = &m_voiceState[voiceId];
      voiceState->m_stolen = voiceState->m_active = false;
      m_traversal.add(m_localIndex[voiceId], m_localVoice[voiceId], unisonIndex, voiceState->m_active,
                      voiceState->m_stolen, allow_glide);
    }
  }
  inline void clear_keyState()
  {
    m_traversal.init();
    for(uint32_t k = 0; k < Keys; k++)
    {
      KeyAssignment* keyState = &m_keyState[k];
      // find active keys
      if(keyState->m_active)
      {
        keyUp_apply(keyState);
        keyUp_confirm(keyState);
      }
    }
  }
  inline void clear_keyState(const AllocatorId _alloc)
  {
    m_traversal.init();
    for(uint32_t k = 0; k < Keys; k++)
    {
      KeyAssignment* keyState = &m_keyState[k];
      // find allocated keys
      if(keyState->m_origin == _alloc)
      {
        keyUp_apply(keyState);
        keyUp_confirm(keyState);
      }
    }
  }
};
