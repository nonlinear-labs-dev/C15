#pragma once

/******************************************************************************/
/** @file       ae_envelopes.h
    @date
    @version    1.7-0
    @author     M. Seeber
    @brief      provide envelope engines
    @note       separate mono and poly engines!
    @todo
*******************************************************************************/

#include <stdint.h>
#include "nltoolbox.h"

namespace Engine
{
  namespace Envelopes
  {
    namespace Proto
    {
      const uint32_t SegmentType_ADBDSR[4][4] = { { 4, 2, 0, 0 }, { 1, 3, 0, 0 }, { 2, 0, 0, 0 }, { 3, 0, 0, 0 } },
                     SegmentType_Gate[4][4] = { { 1, 0, 1, 1 }, { 2, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } },
                     SegmentType_Decay[4][4] = { { 1, 2, 1, 1 }, { 2, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
      constexpr uint32_t SegSize = 5;
      constexpr float renderMin = 1e-9f;

      template <uint32_t Size> struct SingleSegment
      {
        float m_dx[Size] = {}, m_dest_magnitude[Size] = {}, m_curve = 0.0f;
        uint32_t m_state = 0, m_next = 0;
      };

      template <uint32_t Size> struct SplitSegment
      {
        float m_dx[Size] = {}, m_dest_magnitude[Size] = {}, m_dest_timbre[Size] = {}, m_curve = 0.0f;
        uint32_t m_state = 0, m_next = 0;
      };

      struct EnvBody_Single
      {
        float m_x = 0.0f, m_y = 0.0f, m_start_magnitude = 0.0f, m_signal_magnitude = 0.0f;
        uint32_t m_state = 0, m_next = 0, m_index = 0;
      };

      struct EnvBody_Split : EnvBody_Single
      {
        float m_start_timbre = 0.0f, m_signal_timbre = 0.0f;
      };
    }  // namespace Engine::Envelopes::Proto

    template <uint32_t Size> struct SplitEnvelope
    {
      Proto::EnvBody_Split m_body[Size];
      Proto::SplitSegment<Size> m_segment[Proto::SegSize];
      float m_timeFactor[Size][Proto::SegSize - 1] = {}, m_levelFactor[Size] = {};
      float m_peakLevels[Size] = {}, m_splitValues[2] = {};
      const uint32_t m_startIndex = 1, m_stopIndex = 4;
      inline SplitEnvelope()
      {
        for(uint32_t s = 1; s < Proto::SegSize; s++)
        {
          const uint32_t segment = s - 1;
          m_segment[s].m_state = Proto::SegmentType_ADBDSR[segment][0];
          m_segment[s].m_next = Proto::SegmentType_ADBDSR[segment][1];
          for(uint32_t v = 0; v < Size; v++)
          {
            setSegmentDx(v, s, static_cast<float>(Proto::SegmentType_ADBDSR[segment][2]));
            setSegmentDest(v, s, false, static_cast<float>(Proto::SegmentType_ADBDSR[segment][3]));
            m_levelFactor[v] = 0.0f;
            m_timeFactor[v][segment] = 1.0f;
          }
        }
      }
      inline void start(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
        body->m_y = 1.0f - m_segment[m_startIndex].m_dx[_voiceId];
        body->m_start_magnitude = body->m_signal_magnitude;
        body->m_start_timbre = body->m_signal_timbre;
        body->m_state = m_segment[m_startIndex].m_state;
        body->m_index = m_startIndex;
        body->m_next = m_segment[m_startIndex].m_next;
      }
      inline void stop(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        body->m_x = m_segment[m_stopIndex].m_dx[_voiceId];
        body->m_y = 1.0f - m_segment[m_stopIndex].m_dx[_voiceId];
        body->m_start_magnitude = body->m_signal_magnitude;
        body->m_start_timbre = body->m_signal_timbre;
        body->m_state = m_segment[m_stopIndex].m_state;
        body->m_index = m_stopIndex;
        body->m_next = m_segment[m_stopIndex].m_next;
      }
      inline void tick(const uint32_t _voiceId, const float _mute)
      {
        auto body = &m_body[_voiceId];
        const uint32_t segment = body->m_index;
        const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;
        const float diff_timbre = m_segment[segment].m_dest_timbre[_voiceId] - body->m_start_timbre;
        switch(body->m_state)
        {
          case 0:
            break;
          case 1:
            if(body->m_x < 1.0f)
            {

              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
              body->m_signal_timbre = body->m_start_timbre + (diff_timbre * body->m_x);
              body->m_x += m_segment[segment].m_dx[_voiceId];
            }
            else
            {
              body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
              body->m_signal_timbre = m_segment[segment].m_dest_timbre[_voiceId];
              nextSegment(_voiceId);
            }
            break;
          case 2:
            if(body->m_y > Proto::renderMin)
            {
              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.0f - body->m_y));
              body->m_signal_timbre = body->m_start_timbre + (diff_timbre * (1.0f - body->m_y));
              body->m_y *= 1.0f - m_segment[segment].m_dx[_voiceId];
            }
            else
            {
              body->m_signal_magnitude = body->m_start_magnitude + diff_magnitude;
              body->m_signal_timbre = body->m_start_timbre + diff_timbre;
            }
            break;

          case 3:
            if(body->m_y > Proto::renderMin)
            {
              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.0f - body->m_y));
              body->m_signal_timbre = body->m_start_timbre + (diff_timbre * (1.0f - body->m_y));
              body->m_y *= _mute * (1.0f - m_segment[segment].m_dx[_voiceId]);
            }
            else
            {
              body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
              body->m_signal_timbre = m_segment[segment].m_dest_timbre[_voiceId];
              nextSegment(_voiceId);
            }
            break;
          case 4:
            if(body->m_x < 1.0f)
            {
              float x = NlToolbox::Curves::SquaredCurvature(body->m_x, m_segment[segment].m_curve);
              x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
              x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
              x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * x);
              body->m_signal_timbre = body->m_start_timbre + (diff_timbre * x);
              body->m_x += m_segment[segment].m_dx[_voiceId];
            }
            else
            {
              body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
              body->m_signal_timbre = m_segment[segment].m_dest_timbre[_voiceId];
              nextSegment(_voiceId);
            }
            break;
        }
      }
      inline void nextSegment(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        body->m_x = m_segment[body->m_next].m_dx[_voiceId];
        body->m_y = 1.0f - m_segment[body->m_next].m_dx[_voiceId];
        body->m_start_magnitude = body->m_signal_magnitude;
        body->m_start_timbre = body->m_signal_timbre;
        body->m_state = m_segment[body->m_next].m_state;
        body->m_index = body->m_next;
        body->m_next = m_segment[body->m_index].m_next;
      }
      inline void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
      {
        m_segment[_segmentId].m_dx[_voiceId] = _value;
      }
      inline void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const bool _splitMode,
                                 const float _value)
      {
        if(_splitMode)
        {
          m_segment[_segmentId].m_dest_magnitude[_voiceId]
              = NlToolbox::Crossfades::unipolarCrossFade(_value, m_peakLevels[_voiceId], m_splitValues[0]);
          m_segment[_segmentId].m_dest_timbre[_voiceId]
              = NlToolbox::Crossfades::unipolarCrossFade(_value, m_peakLevels[_voiceId], m_splitValues[1]);
        }
        else
        {
          m_segment[_segmentId].m_dest_magnitude[_voiceId] = _value;
          m_segment[_segmentId].m_dest_timbre[_voiceId] = _value;
        }
      }
      inline void setSplitValue(const float _value)
      {
        m_splitValues[0] = std::max(0.0f, _value);
        m_splitValues[1] = std::max(0.0f, -_value);
      }
      inline void setAttackCurve(const float _value)
      {
        m_segment[1].m_curve = _value;
      }
      inline void setPeakLevel(const uint32_t _voiceId, const float _value)
      {
        m_peakLevels[_voiceId] = _value;
      }
      inline void reset()
      {
        for(uint32_t v = 0; v < Size; v++)
        {
          m_body[v].m_index = 0;
          m_body[v].m_x = 0.0f;
          m_body[v].m_signal_timbre = 0.0f;
          m_body[v].m_start_timbre = 0.0f;
          m_body[v].m_signal_magnitude = 0.0f;
          m_body[v].m_start_magnitude = 0.0f;
        }
      }
    };

    template <uint32_t Size> struct RetriggerEnvelope
    {
      Proto::EnvBody_Single m_body[Size];
      Proto::SingleSegment<Size> m_segment[Proto::SegSize];
      float m_timeFactor[Size][Proto::SegSize - 1] = {}, m_levelFactor[Size] = {}, m_clipFactor[Size] = {};
      const uint32_t m_startIndex = 1, m_stopIndex = 4;
      float m_retriggerHardness = 0.0f;
      inline RetriggerEnvelope()
      {
        for(uint32_t s = 1; s < Proto::SegSize; s++)
        {
          const uint32_t segment = s - 1;
          m_segment[s].m_state = Proto::SegmentType_ADBDSR[segment][0];
          m_segment[s].m_next = Proto::SegmentType_ADBDSR[segment][1];
          for(uint32_t v = 0; v < Size; v++)
          {
            setSegmentDx(v, s, static_cast<float>(Proto::SegmentType_ADBDSR[segment][2]));
            setSegmentDest(v, s, static_cast<float>(Proto::SegmentType_ADBDSR[segment][3]));
            m_levelFactor[v] = 0.0f;
            m_clipFactor[v] = 1.0f;
            m_timeFactor[v][segment] = 1.0f;
          }
        }
      }
      inline void start(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
        body->m_y = 1.0f - m_segment[m_startIndex].m_dx[_voiceId];
        body->m_start_magnitude = (1.0f - m_retriggerHardness) * body->m_signal_magnitude;
        body->m_state = m_segment[m_startIndex].m_state;
        body->m_index = m_startIndex;
        body->m_next = m_segment[m_startIndex].m_next;
      }
      inline void stop(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        body->m_x = m_segment[m_stopIndex].m_dx[_voiceId];
        body->m_y = 1.0f - m_segment[m_stopIndex].m_dx[_voiceId];
        body->m_start_magnitude = body->m_signal_magnitude;
        body->m_state = m_segment[m_stopIndex].m_state;
        body->m_index = m_stopIndex;
        body->m_next = m_segment[m_stopIndex].m_next;
      }
      inline void tick(const uint32_t _voiceId, const float _mute)
      {
        auto body = &m_body[_voiceId];
        const uint32_t segment = body->m_index;
        const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;
        switch(body->m_state)
        {
          case 0:
            break;
          case 1:
            if(body->m_x < 1.0f)
            {
              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
              body->m_x += m_segment[segment].m_dx[_voiceId];
            }
            else
            {
              body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
              nextSegment(_voiceId);
            }
            break;
          case 2:
            if(body->m_y > Proto::renderMin)
            {
              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.0f - body->m_y));
              body->m_y *= 1.0f - m_segment[segment].m_dx[_voiceId];
            }
            else
            {
              body->m_signal_magnitude = body->m_start_magnitude + diff_magnitude;
            }
            break;
          case 3:
            if(body->m_y > Proto::renderMin)
            {
              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.0f - body->m_y));
              body->m_y *= _mute * (1.0f - m_segment[segment].m_dx[_voiceId]);
            }
            else
            {
              body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
              nextSegment(_voiceId);
            }
            break;
          case 4:
            if(body->m_x < 1.0f)
            {
              float x = NlToolbox::Curves::SquaredCurvature(body->m_x, m_segment[segment].m_curve);
              x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
              x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
              x = NlToolbox::Curves::SquaredCurvature(x, m_segment[segment].m_curve);
              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * x);
              body->m_x += m_segment[segment].m_dx[_voiceId];
            }
            else
            {
              body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
              nextSegment(_voiceId);
            }
            break;
        }
      }
      inline void nextSegment(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        body->m_x = m_segment[body->m_next].m_dx[_voiceId];
        body->m_y = 1.0f - m_segment[body->m_next].m_dx[_voiceId];
        body->m_start_magnitude = body->m_signal_magnitude;
        body->m_state = m_segment[body->m_next].m_state;
        body->m_index = body->m_next;
        body->m_next = m_segment[body->m_index].m_next;
      }
      inline void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
      {
        m_segment[_segmentId].m_dx[_voiceId] = _value;
      }
      inline void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
      {
        m_segment[_segmentId].m_dest_magnitude[_voiceId] = _value;
      }
      inline void setAttackCurve(const float _value)
      {
        m_segment[1].m_curve = _value;
      }
      inline void setRetriggerHardness(const float _value)
      {
        m_retriggerHardness = _value;
      }
      inline void reset()
      {
        for(uint32_t v = 0; v < Size; v++)
        {
          m_body[v].m_index = 0;
          m_body[v].m_x = 0.0f;
          m_body[v].m_signal_magnitude = 0.0f;
          m_body[v].m_start_magnitude = 0.0f;
        }
      }
    };

    template <uint32_t Size> struct GateEnvelope
    {
      const uint32_t m_startIndex = 1, m_stopIndex = 2;
      Proto::EnvBody_Single m_body[Size];
      Proto::SingleSegment<Size> m_segment[Proto::SegSize];
      inline GateEnvelope()
      {
        for(uint32_t s = 1; s < Proto::SegSize; s++)
        {
          const uint32_t segment = s - 1;
          m_segment[s].m_state = Proto::SegmentType_Gate[segment][0];
          m_segment[s].m_next = Proto::SegmentType_Gate[segment][1];
          for(uint32_t v = 0; v < Size; v++)
          {
            setSegmentDx(v, s, static_cast<float>(Proto::SegmentType_Gate[segment][2]));
            setSegmentDest(v, s, static_cast<float>(Proto::SegmentType_Gate[segment][3]));
          }
        }
      }
      inline void init(const float _release)
      {
        for(uint32_t v = 0; v < Size; v++)
        {
          setSegmentDx(v, 2, _release);
        }
      }
      inline void start(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
        body->m_y = 1.0f - m_segment[m_startIndex].m_dx[_voiceId];
        body->m_start_magnitude = body->m_signal_magnitude;
        body->m_state = m_segment[m_startIndex].m_state;
        body->m_index = m_startIndex;
        body->m_next = m_segment[m_startIndex].m_next;
      }
      inline void stop(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        body->m_x = m_segment[m_stopIndex].m_dx[_voiceId];
        body->m_y = 1.0f - m_segment[m_stopIndex].m_dx[_voiceId];
        body->m_start_magnitude = body->m_signal_magnitude;
        body->m_state = m_segment[m_stopIndex].m_state;
        body->m_index = m_stopIndex;
        body->m_next = m_segment[m_stopIndex].m_next;
      }
      inline void tick(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        const uint32_t segment = body->m_index;
        const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;
        switch(body->m_state)
        {
          case 0:
            break;
          case 1:
            if(body->m_x < 1.0f)
            {
              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
              body->m_x += m_segment[segment].m_dx[_voiceId];
            }
            else
            {
              body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
              nextSegment(_voiceId);
            }
            break;
          case 2:
            if(body->m_y > Proto::renderMin)
            {
              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.0f - body->m_y));
              body->m_y *= 1.0f - m_segment[segment].m_dx[_voiceId];
            }
            else
            {
              body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
              nextSegment(_voiceId);
            }
            break;
        }
      }
      inline void nextSegment(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        body->m_x = m_segment[body->m_next].m_dx[_voiceId];
        body->m_y = 1.0f - m_segment[body->m_next].m_dx[_voiceId];
        body->m_start_magnitude = body->m_signal_magnitude;
        body->m_state = m_segment[body->m_next].m_state;
        body->m_index = body->m_next;
        body->m_next = m_segment[body->m_index].m_next;
      }
      inline void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
      {
        m_segment[_segmentId].m_dx[_voiceId] = _value;
      }
      inline void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
      {
        m_segment[_segmentId].m_dest_magnitude[_voiceId] = _value;
      }
      inline void reset()
      {
        for(uint32_t v = 0; v < Size; v++)
        {
          m_body[v].m_index = 0;
          m_body[v].m_x = 0.0f;
          m_body[v].m_signal_magnitude = 0.0f;
          m_body[v].m_start_magnitude = 0.0f;
        }
      }
    };

    template <uint32_t Size> struct DecayEnvelope
    {
      const uint32_t m_startIndex = 1;
      Proto::EnvBody_Single m_body[Size];
      Proto::SingleSegment<Size> m_segment[Proto::SegSize];
      inline DecayEnvelope()
      {
        for(uint32_t s = 1; s < Proto::SegSize; s++)
        {
          const uint32_t segment = s - 1;
          m_segment[s].m_state = Proto::SegmentType_Decay[segment][0];
          m_segment[s].m_next = Proto::SegmentType_Decay[segment][1];
          for(uint32_t v = 0; v < Size; v++)
          {
            setSegmentDx(v, s, static_cast<float>(Proto::SegmentType_Decay[segment][2]));
            setSegmentDest(v, s, static_cast<float>(Proto::SegmentType_Decay[segment][3]));
          }
        }
      }
      inline void start(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        body->m_x = m_segment[m_startIndex].m_dx[_voiceId];
        body->m_y = 1.0f - m_segment[m_startIndex].m_dx[_voiceId];
        body->m_start_magnitude = body->m_signal_magnitude;
        body->m_state = m_segment[m_startIndex].m_state;
        body->m_index = m_startIndex;
        body->m_next = m_segment[m_startIndex].m_next;
      }
      inline void tick(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        const uint32_t segment = body->m_index;
        const float diff_magnitude = m_segment[segment].m_dest_magnitude[_voiceId] - body->m_start_magnitude;
        switch(body->m_state)
        {
          case 0:
            break;
          case 1:
            if(body->m_x < 1.0f)
            {
              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * body->m_x);
              body->m_x += m_segment[segment].m_dx[_voiceId];
            }
            else
            {
              body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
              nextSegment(_voiceId);
            }
            break;
          case 2: /* exponential, quasi-finite rendering (release phase) */
            if(body->m_y > Proto::renderMin)
            {
              body->m_signal_magnitude = body->m_start_magnitude + (diff_magnitude * (1.0f - body->m_y));
              body->m_y *= 1.0f - m_segment[segment].m_dx[_voiceId];
            }
            else
            {
              body->m_signal_magnitude = m_segment[segment].m_dest_magnitude[_voiceId];
              nextSegment(_voiceId);
            }
            break;
        }
      }
      inline void nextSegment(const uint32_t _voiceId)
      {
        auto body = &m_body[_voiceId];
        body->m_x = m_segment[body->m_next].m_dx[_voiceId];
        body->m_y = 1.0f - m_segment[body->m_next].m_dx[_voiceId];
        body->m_start_magnitude = body->m_signal_magnitude;
        body->m_state = m_segment[body->m_next].m_state;
        body->m_index = body->m_next;
        body->m_next = m_segment[body->m_index].m_next;
      }
      inline void setSegmentDx(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
      {
        m_segment[_segmentId].m_dx[_voiceId] = _value;
      }
      inline void setSegmentDest(const uint32_t _voiceId, const uint32_t _segmentId, const float _value)
      {
        m_segment[_segmentId].m_dest_magnitude[_voiceId] = _value;
      }
      inline void reset()
      {
        for(uint32_t v = 0; v < Size; v++)
        {
          m_body[v].m_index = 0;
          m_body[v].m_x = 0.0f;
          m_body[v].m_signal_magnitude = 0.0f;
          m_body[v].m_start_magnitude = 0.0f;
        }
      }
    };
  }  // namespace Engine::Envelopes
}  // namespace Engine
