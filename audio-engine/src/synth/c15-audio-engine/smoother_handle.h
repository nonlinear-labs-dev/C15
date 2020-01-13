#pragma once

/******************************************************************************/
/** @file       smoother_handle.h
    @date
    @version    1.7-3
    @author     M. Seeber
    @brief      provide smoothing functionality and abstraction in order to
                perform smoothing within a section
    @note       in theory, we could consider using parallel_data types for the
                renderers (which maybe would boost performance?)
    @todo
*******************************************************************************/

#include "parameter-db/generated/c15_config.h"

// basic smoothing unit
class ProtoSmoother
{
 public:
  // smoothed value
  float m_value = 0.0f, m_dx = 0.0f;
  // constructor
  inline ProtoSmoother()
  {
  }
  // starting a segment (_dx: converted time, _dest: destination)
  inline void start(const float _dx, const float _dest)
  {
    if(_dest != m_value)
    {
      m_state = true;
      m_start = m_value;
      m_diff = _dest - m_start;
      m_x = m_dx = _dx;
    }
  }
  // syncing
  inline void sync(const float _dest)
  {
    m_state = false;
    m_value = _dest;
  }
  // rendering
  inline void render()
  {
    if(m_state)
    {
      if(m_x >= 1.0f)
      {
        m_state = false;
        m_x = 1.0f;
      }
      m_value = m_start + (m_x * m_diff);
      m_x += m_dx;
    }
  }
  // reset (if needed)
  inline void reset()
  {
    m_state = false;
    m_value = 0.0f;
  }

 private:
  // segment-specific private variables
  float m_start, m_diff, m_x;
  bool m_state = false;
};

// smoother copy handle (for parameter smoothers that directly translate to signals)
template <uint32_t Length> struct SmootherCopyHandle
{
  uint32_t m_smootherId[Length] = {}, m_signalId[Length] = {}, m_length = {};
  inline void add_copy_id(const uint32_t _smootherId, const uint32_t _signalId)
  {
    m_smootherId[m_length] = _smootherId;
    m_signalId[m_length] = _signalId;
    m_length++;
  }
};

// sync copy handle (for sync-type parameters that directly translate to signals)
template <uint32_t Length> struct SyncCopyHandle
{
  uint32_t m_signalId[Length] = {};
  bool m_smootherId[Length] = {};
  inline void add_copy_id(const uint32_t _smootherId, const uint32_t _signalId)
  {
    m_smootherId[_smootherId] = true;
    m_signalId[_smootherId] = _signalId;
  }
};

// smoother handle abstraction (includes smoother arrays for each clock type)
template <class Sync, class Audio, class Fast, class Slow> class SmootherHandle
{
 public:
  // copy handles
  SmootherCopyHandle<static_cast<uint32_t>(Audio::_LENGTH_)> m_copy_audio;
  SmootherCopyHandle<static_cast<uint32_t>(Fast::_LENGTH_)> m_copy_fast;
  SmootherCopyHandle<static_cast<uint32_t>(Slow::_LENGTH_)> m_copy_slow;
  SyncCopyHandle<static_cast<uint32_t>(Sync::_LENGTH_)> m_copy_sync;
  // constructor
  inline SmootherHandle()
  {
  }
  // start methods according to clock type
  inline void start_sync(const uint32_t _id, const float _dest)
  {
    m_sync[_id] = _dest;
  }
  inline void start_audio(const uint32_t _id, const float _dx, const float _dest)
  {
    m_audio[_id].start(_dx, _dest);
  }
  inline void start_fast(const uint32_t _id, const float _dx, const float _dest)
  {
    m_fast[_id].start(_dx, _dest);
  }
  inline void start_slow(const uint32_t _id, const float _dx, const float _dest)
  {
    m_slow[_id].start(_dx, _dest);
  }
  // render methods according to clock type
  inline void render_audio()
  {
    for(uint32_t i = 0; i < static_cast<uint32_t>(Audio::_LENGTH_); i++)
    {
      m_audio[i].render();
    }
  }
  inline void render_fast()
  {
    for(uint32_t i = 0; i < static_cast<uint32_t>(Fast::_LENGTH_); i++)
    {
      m_fast[i].render();
    }
  }
  inline void render_slow()
  {
    for(uint32_t i = 0; i < static_cast<uint32_t>(Slow::_LENGTH_); i++)
    {
      m_slow[i].render();
    }
  }
  // get methods according to clock type (in case of run-time-related access)
  inline float get_sync(const uint32_t _id)
  {
    return m_sync[_id];
  }
  inline float get_audio(const uint32_t _id)
  {
    return m_audio[_id].m_value;
  }
  inline float get_fast(const uint32_t _id)
  {
    return m_fast[_id].m_value;
  }
  inline float get_slow(const uint32_t _id)
  {
    return m_slow[_id].m_value;
  }
  // get method (auto-deducing clock type, in case of compile-time-related access)
  inline float get(const Sync _id)
  {
    return m_sync[static_cast<uint32_t>(_id)];
  }
  inline float get(const Audio _id)
  {
    return m_audio[static_cast<uint32_t>(_id)].m_value;
  }
  inline float get(const Fast _id)
  {
    return m_fast[static_cast<uint32_t>(_id)].m_value;
  }
  inline float get(const Slow _id)
  {
    return m_slow[static_cast<uint32_t>(_id)].m_value;
  }
  // reset method (if needed)
  inline void reset()
  {
    for(uint32_t i = 0; i < static_cast<uint32_t>(Sync::_LENGTH_); i++)
    {
      m_sync[i] = 0.0f;
    }
    for(uint32_t i = 0; i < static_cast<uint32_t>(Audio::_LENGTH_); i++)
    {
      m_audio[i].reset();
    }
    for(uint32_t i = 0; i < static_cast<uint32_t>(Fast::_LENGTH_); i++)
    {
      m_fast[i].reset();
    }
    for(uint32_t i = 0; i < static_cast<uint32_t>(Slow::_LENGTH_); i++)
    {
      m_slow[i].reset();
    }
  }

 private:
  // private smoother arrays for each clock type
  ProtoSmoother m_audio[static_cast<uint32_t>(Audio::_LENGTH_)], m_fast[static_cast<uint32_t>(Fast::_LENGTH_)],
      m_slow[static_cast<uint32_t>(Slow::_LENGTH_)] = {};
  float m_sync[static_cast<uint32_t>(Sync::_LENGTH_)] = {};
};

namespace Engine
{
  namespace Handle
  {
    struct Time_Handle
    {
      float m_convert[4] = {}, m_dx_audio = 0.0f, m_dx_fast = 0.0f, m_dx_slow = 0.0f, m_millisecond = 0.0f,
            m_sample_inc = 0.0f;
      inline void init(const uint32_t _samplerate)
      {
        float rate = static_cast<float>(_samplerate);
        m_millisecond = 1e-3f * rate;
        m_sample_inc = 1.0f / rate;
        m_convert[1] = 1.0f;
        m_convert[2] = rate / static_cast<float>(C15::Config::clock_rates[0][1]);
        m_convert[3] = rate / static_cast<float>(C15::Config::clock_rates[0][2]);
      }
      inline float clip(const float _value)
      {
        return _value > 1.0f ? 1.0f : _value;
      }
      inline float eval_ms(const uint32_t _type, const float _value)
      {
        return clip(m_convert[_type] / ((_value * m_millisecond) + 1.0f));
      }
      inline void update_ms(const float _value)
      {
        m_dx_audio = eval_ms(1, _value);
        m_dx_fast = eval_ms(2, _value);
        m_dx_slow = eval_ms(3, _value);
      }
    };
  }
}
