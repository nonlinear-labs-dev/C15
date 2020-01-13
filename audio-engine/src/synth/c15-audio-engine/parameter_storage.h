#pragma once

/******************************************************************************/
/** @file       parameter_storage.h
    @date
    @version    1.7-0
    @author     M. Seeber
    @brief      abstractions for parameters
                (currently, all parameters operate completely event-based)
    @todo
*******************************************************************************/

#include "parameter-db/generated/c15_config.h"
#include "parameter-db/generated/parameter_info.h"
#include "macro_assignment.h"

// first run - prototypical stuff

namespace Engine
{

  namespace Proto
  {

    struct Parameter
    {
      float m_position = 0.0f, m_initial = 0.0f;
      inline bool update_position(const float _position)
      {
        if(_position != m_position)
        {
          m_position = _position;
          return true;
        }
        return false;
      }
    };

    struct Time_Aspect
    {
      float m_dx_audio = 0.0f, m_dx_fast = 0.0f, m_dx_slow = 0.0f;
    };

    template <typename T_Scale> struct Scale_Aspect
    {
      T_Scale m_id;
      float m_factor = 0.0f, m_offset = 0.0f;
    };

    template <typename T_Section, typename T_Clock> struct Render_Aspect
    {
      T_Section m_section;
      T_Clock m_clock;
      uint32_t m_index = 0;
    };

  }  // namespace Engine::Proto

}  // namespace Engine

// primitive aspects

using Time_Aspect = Engine::Proto::Time_Aspect;
using Scale_Aspect = Engine::Proto::Scale_Aspect<C15::Properties::SmootherScale>;
using Render_Aspect = Engine::Proto::Render_Aspect<C15::Descriptors::SmootherSection, C15::Descriptors::SmootherClock>;

// second run - parameter stuff

namespace Engine
{

  namespace Parameters
  {

    struct Hardware_Source_Parameter : Proto::Parameter
    {
      C15::Properties::HW_Return_Behavior m_behavior = C15::Properties::HW_Return_Behavior::Stay;
      uint32_t m_offset = 0;
      inline bool update_behavior(const C15::Properties::HW_Return_Behavior _behavior)
      {
        if(_behavior != m_behavior)
        {
          m_behavior = _behavior;
          return true;
        }
        return false;
      }
    };

    struct Hardware_Amount_Parameter : Proto::Parameter
    {
      uint32_t m_sourceId = 0;
    };

    struct Setting_Parameter : Proto::Parameter
    {
      Scale_Aspect m_scaling;
      float m_scaled = 0.0f;
      inline void init(const C15::Properties::SmootherScale _id, const float _factor, const float _offset,
                       const float _value)
      {
        m_scaling.m_id = _id;
        m_scaling.m_factor = _factor;
        m_scaling.m_offset = _offset;
        m_position = _value;
      }
    };

    struct Time_Parameter : Setting_Parameter
    {
      Time_Aspect m_dx;
    };

    struct Macro_Control_Parameter : Proto::Parameter
    {
      Time_Parameter m_time;
      float m_unclipped = 0.0f;
      uint32_t m_id = 0, m_index = 0;
    };

    struct Modulateable_Parameter : Proto::Parameter
    {
      float m_amount = 0.0f, m_base = 0.0f, m_ceil = 0.0f, m_unclipped = 0.0f, m_scaled = 0.0f;
      C15::Parameters::Macro_Controls m_source = C15::Parameters::Macro_Controls::None;
      Scale_Aspect m_scaling;
      Render_Aspect m_render;
      bool m_polarity = false, m_splitpoint = false;
      inline bool update_source(const C15::Parameters::Macro_Controls _source)
      {
        if(_source != m_source)
        {
          m_source = _source;
          return true;
        }
        return false;
      }
      inline bool update_amount(const float _amount)
      {
        if(_amount != m_amount)
        {
          m_amount = _amount;
          return true;
        }
        return false;
      }
      inline float depolarize(const float _value)
      {
        if(m_polarity)
        {
          return (0.5f * _value) + 0.5f;
        }
        return _value;
      }
      inline float polarize(const float _value)
      {
        if(m_polarity)
        {
          return (2.0f * _value) - 1.0f;
        }
        return _value;
      }
      inline bool modulate(const float _mod)
      {
        m_unclipped = m_base + (m_amount * _mod);
        return m_position != m_unclipped;  // basic change detection (position needs to be updated from outside)
      }
      inline void update_modulation_aspects(const float _mod)
      {
        m_base = m_position - (m_amount * _mod);
        m_ceil = m_base + m_amount;
      }
    };

    struct Unmodulateable_Parameter : Proto::Parameter
    {
      float m_scaled = 0.0f;
      Scale_Aspect m_scaling;
      Render_Aspect m_render;
    };

  }  // namespace Engine::Parameters

}  // namespace Engine

// parameter types

using HW_Src_Param = Engine::Parameters::Hardware_Source_Parameter;
using HW_Amt_Param = Engine::Parameters::Hardware_Amount_Parameter;
using Macro_Param = Engine::Parameters::Macro_Control_Parameter;
using Direct_Param = Engine::Parameters::Unmodulateable_Parameter;
using Target_Param = Engine::Parameters::Modulateable_Parameter;
using Time_Param = Engine::Parameters::Time_Parameter;
using Setting_Param = Engine::Parameters::Setting_Parameter;

// third run - storage

namespace Engine
{

  namespace Storage
  {

    template <class HW, class HA, class MC, class TP, class DP> struct Global_Parameter_Storage
    {
      HW_Src_Param m_source[static_cast<uint32_t>(HW::_LENGTH_)];
      HW_Amt_Param m_amount[static_cast<uint32_t>(HA::_LENGTH_)];
      Macro_Param m_macro[static_cast<uint32_t>(MC::_LENGTH_)];
      Target_Param m_target[static_cast<uint32_t>(TP::_LENGTH_)];
      Direct_Param m_direct[static_cast<uint32_t>(DP::_LENGTH_)];
      MC_Assignment<MC, TP> m_assignment;
      const uint32_t m_source_count
          = static_cast<uint32_t>(HW::_LENGTH_),
          m_amount_count = static_cast<uint32_t>(HA::_LENGTH_), m_macro_count = static_cast<uint32_t>(MC::_LENGTH_),
          m_target_count = static_cast<uint32_t>(TP::_LENGTH_), m_direct_count = static_cast<uint32_t>(DP::_LENGTH_);
    };

    template <class MC, class TP, class DP> struct Layer_Parameter_Storage
    {
      Target_Param m_target[static_cast<uint32_t>(TP::_LENGTH_)];
      Direct_Param m_direct[static_cast<uint32_t>(DP::_LENGTH_)];
      MC_Assignment<MC, TP> m_assignment;
      const uint32_t m_target_count = static_cast<uint32_t>(TP::_LENGTH_),
                     m_direct_count = static_cast<uint32_t>(DP::_LENGTH_);
    };

  }  // namespace Engine::Storage

}  // namespace Engine

// storage types

using Global_Storage
    = Engine::Storage::Global_Parameter_Storage<C15::Parameters::Hardware_Sources, C15::Parameters::Hardware_Amounts,
                                                C15::Parameters::Macro_Controls, C15::Parameters::Global_Modulateables,
                                                C15::Parameters::Global_Unmodulateables>;

using Layer_Storage
    = Engine::Storage::Layer_Parameter_Storage<C15::Parameters::Macro_Controls, C15::Parameters::Local_Modulateables,
                                               C15::Parameters::Local_Unmodulateables>;
