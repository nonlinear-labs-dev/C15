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

#include <c15_config.h>
#include <algorithm>
#include "ParallelData.h"

// basic smoothing unit
// - state-less, but normalized
template <size_t Size> class ProtoSmoother
{
 public:
  // smoothed value
  static constexpr size_t ParallelData_Padding(const size_t& _size)
  {
    return _size % 4 > 0 ? _size + 4 - (_size % 4) : _size;
    // - pad arbitrary ParallelData sizes (they have to equal multiples of four)
  }
  ParallelData<float, ParallelData_Padding(Size)> m_value{ 0.0f };

  // starting a segment (_dx: converted time, _dest: destination)
  inline void start(int idx, const float _dx, const float _dest)
  {
    if(_dest != m_value[idx])
    {
      m_start[idx] = m_value[idx];
      m_diff[idx] = _dest - m_start[idx];
      m_x[idx] = m_dx[idx] = _dx;
    }
  }
  // syncing
  inline void sync(int idx, const float _dest)
  {
    m_start[idx] = m_value[idx] = _dest;
    m_x[idx] = m_dx[idx] = m_diff[idx] = 0.0f;
  }

  inline void timeUpdate(int idx, const float _dx)
  {
    m_dx[idx] = _dx;
  }

  // rendering
  inline void render()
  {
    m_x = std::min(m_x, 1.0f);
    m_value = m_start + (m_x * m_diff);
    m_x += m_dx;  // x should stay bound (x > 1 && x < 2)
  }

 private:
  // segment-specific private variables
  ParallelData<float, ParallelData_Padding(Size)> m_start{ 0.0f };
  ParallelData<float, ParallelData_Padding(Size)> m_diff{ 0.0f };
  ParallelData<float, ParallelData_Padding(Size)> m_x{ 0.0f };
  ParallelData<float, ParallelData_Padding(Size)> m_dx{ 0.0f };
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

  // start methods according to clock type
  inline void start_sync(const uint32_t _id, const float _dest)
  {
    m_sync[_id] = _dest;
  }
  inline void start_audio(const uint32_t _id, const float _dx, const float _dest)
  {
    m_audio.start(_id, _dx, _dest);
  }
  inline void start_fast(const uint32_t _id, const float _dx, const float _dest)
  {
    m_fast.start(_id, _dx, _dest);
  }
  inline void start_slow(const uint32_t _id, const float _dx, const float _dest)
  {
    m_slow.start(_id, _dx, _dest);
  }
  // render methods according to clock type
  inline void render_audio()
  {
    m_audio.render();
  }
  inline void render_fast()
  {
    m_fast.render();
  }
  inline void render_slow()
  {
    m_slow.render();
  }
  // get methods according to clock type (in case of run-time-related access)
  inline float get_sync(const uint32_t _id)
  {
    return m_sync[_id];
  }
  inline float get_audio(const uint32_t _id)
  {
    return m_audio.m_value[_id];
  }
  inline float get_fast(const uint32_t _id)
  {
    return m_fast.m_value[_id];
  }
  inline float get_slow(const uint32_t _id)
  {
    return m_slow.m_value[_id];
  }

  // get method (auto-deducing clock type, in case of compile-time-related access)
  inline float get(const Sync _id)
  {
    return m_sync[static_cast<uint32_t>(_id)];
  }
  inline float get(const Audio _id)
  {
    return m_audio.m_value[static_cast<uint32_t>(_id)];
  }
  inline float get(const Fast _id)
  {
    return m_fast.m_value[static_cast<uint32_t>(_id)];
  }
  inline float get(const Slow _id)
  {
    return m_slow.m_value[static_cast<uint32_t>(_id)];
  }

  inline void sync(Slow id, float v)
  {
    m_slow.sync(static_cast<uint32_t>(id), v);
  }

  inline void start(Slow id, float d, float v)
  {
    m_slow.start(static_cast<uint32_t>(id), d, v);
  }

 private:
  // private smoother arrays for each clock type
  ProtoSmoother<static_cast<size_t>(Audio::_LENGTH_)> m_audio;
  ProtoSmoother<static_cast<size_t>(Fast::_LENGTH_)> m_fast;
  ProtoSmoother<static_cast<size_t>(Slow::_LENGTH_)> m_slow;
  ParallelData<float, static_cast<uint32_t>(Sync::_LENGTH_)> m_sync{};
};

namespace Engine
{
  namespace Handle
  {
    struct Time_Handle
    {
      float m_convert[4] = {}, m_dx_audio = 0.0f, m_dx_fast = 0.0f, m_dx_slow = 0.0f, m_millisecond = 0.0f,
            m_sample_inc = 0.0f;
      inline void init(const uint32_t _upsampleIndex)
      {
        float rate = static_cast<float>(C15::Config::clock_rates[_upsampleIndex][0]);
        m_millisecond = 1e-3f * rate;
        m_sample_inc = 1.0f / rate;
        m_convert[1] = 1.0f;
        m_convert[2] = rate / static_cast<float>(C15::Config::clock_rates[_upsampleIndex][1]);
        m_convert[3] = rate / static_cast<float>(C15::Config::clock_rates[_upsampleIndex][2]);
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
