#pragma once

/******************************************************************************/
/** @file       parameter_descriptor.h
    @date       2020-12-05, 14:53
    @version    1.7B-8
    @author     M. Seeber
    @brief      provide a flexible descriptor structure
    @todo
*******************************************************************************/

#include "parameter_info.h"
#include <stdint.h>

namespace C15
{

  constexpr uint32_t None = 0;

  namespace Descriptors
  {

    struct TypeDescriptor
    {
      const ParameterType m_type;
      const uint32_t m_index;
      // constructors
      inline TypeDescriptor()
          : m_type(ParameterType::None)
          , m_index(None)
      {
      }
      inline TypeDescriptor(const uint32_t _param)
          : m_type(ParameterType::None)
          , m_index(_param)
      {
      }
      inline TypeDescriptor(const Parameters::Hardware_Sources _param)
          : m_type(ParameterType::Hardware_Source)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline TypeDescriptor(const Parameters::Hardware_Amounts _param)
          : m_type(ParameterType::Hardware_Amount)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline TypeDescriptor(const Parameters::Macro_Controls _param)
          : m_type(ParameterType::Macro_Control)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline TypeDescriptor(const Parameters::Macro_Times _param)
          : m_type(ParameterType::Macro_Time)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline TypeDescriptor(const Parameters::Global_Modulateables _param)
          : m_type(ParameterType::Global_Modulateable)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline TypeDescriptor(const Parameters::Global_Unmodulateables _param)
          : m_type(ParameterType::Global_Unmodulateable)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline TypeDescriptor(const Parameters::Local_Modulateables _param)
          : m_type(ParameterType::Local_Modulateable)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline TypeDescriptor(const Parameters::Local_Unmodulateables _param)
          : m_type(ParameterType::Local_Unmodulateable)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
    };

    struct SmootherDescriptor
    {
      const SmootherSection m_section;
      const SmootherClock m_clock;
      const uint32_t m_index;
      // constructors
      inline SmootherDescriptor()
          : m_section(SmootherSection::None)
          , m_clock(SmootherClock::Sync)
          , m_index(None)
      {
      }
      inline SmootherDescriptor(const uint32_t _param)
          : m_section(SmootherSection::None)
          , m_clock(SmootherClock::Sync)
          , m_index(_param)
      {
      }
      inline SmootherDescriptor(const Smoothers::Global_Sync _param)
          : m_section(SmootherSection::Global)
          , m_clock(SmootherClock::Sync)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SmootherDescriptor(const Smoothers::Global_Audio _param)
          : m_section(SmootherSection::Global)
          , m_clock(SmootherClock::Audio)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SmootherDescriptor(const Smoothers::Global_Fast _param)
          : m_section(SmootherSection::Global)
          , m_clock(SmootherClock::Fast)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SmootherDescriptor(const Smoothers::Global_Slow _param)
          : m_section(SmootherSection::Global)
          , m_clock(SmootherClock::Slow)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SmootherDescriptor(const Smoothers::Poly_Sync _param)
          : m_section(SmootherSection::Poly)
          , m_clock(SmootherClock::Sync)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SmootherDescriptor(const Smoothers::Poly_Audio _param)
          : m_section(SmootherSection::Poly)
          , m_clock(SmootherClock::Audio)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SmootherDescriptor(const Smoothers::Poly_Fast _param)
          : m_section(SmootherSection::Poly)
          , m_clock(SmootherClock::Fast)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SmootherDescriptor(const Smoothers::Poly_Slow _param)
          : m_section(SmootherSection::Poly)
          , m_clock(SmootherClock::Slow)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SmootherDescriptor(const Smoothers::Mono_Sync _param)
          : m_section(SmootherSection::Mono)
          , m_clock(SmootherClock::Sync)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SmootherDescriptor(const Smoothers::Mono_Audio _param)
          : m_section(SmootherSection::Mono)
          , m_clock(SmootherClock::Audio)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SmootherDescriptor(const Smoothers::Mono_Fast _param)
          : m_section(SmootherSection::Mono)
          , m_clock(SmootherClock::Fast)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SmootherDescriptor(const Smoothers::Mono_Slow _param)
          : m_section(SmootherSection::Mono)
          , m_clock(SmootherClock::Slow)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
    };

    struct SignalDescriptor
    {
      const ParameterSignal m_signal;
      const uint32_t m_index;
      // constructors
      inline SignalDescriptor()
          : m_signal(ParameterSignal::None)
          , m_index(None)
      {
      }
      inline SignalDescriptor(const uint32_t _param)
          : m_signal(ParameterSignal::None)
          , m_index(_param)
      {
      }
      inline SignalDescriptor(const Signals::Global_Signals _param)
          : m_signal(ParameterSignal::Global_Signal)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SignalDescriptor(const Signals::Quasipoly_Signals _param)
          : m_signal(ParameterSignal::Quasipoly_Signal)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SignalDescriptor(const Signals::Truepoly_Signals _param)
          : m_signal(ParameterSignal::Truepoly_Signal)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
      inline SignalDescriptor(const Signals::Mono_Signals _param)
          : m_signal(ParameterSignal::Mono_Signal)
          , m_index(static_cast<uint32_t>(_param))
      {
      }
    };

    // AE and PG Descriptors

    struct AudioEngineParamDescriptor
    {
      SmootherDescriptor m_smoother;
      SignalDescriptor m_signal;
      Properties::SmootherScale m_scaleId = Properties::SmootherScale::None;
      float m_scaleFactor = 0.0f, m_scaleOffset = 0.0f;
      bool m_polarity = false;
    };

    struct PlaygroundParamDescriptor
    {
      const uint32_t m_coarse_cp = 0, m_fine_cp = 0, m_coarse_amt = 0, m_fine_amt = 0;
      const char *m_inactive_cp = nullptr, *m_group_label_long = nullptr, *m_group_label_short = nullptr,
                 *m_param_label_long = nullptr, *m_param_label_short = nullptr, *m_param_info = nullptr;
    };

  }  // namespace C15::Descriptors

  struct ParameterDescriptor
  {
    Descriptors::TypeDescriptor m_param;
    PID::ParameterID m_param_id = PID::None;
    float m_initial = 0.0f;
    Descriptors::AudioEngineParamDescriptor m_ae;
    Descriptors::PlaygroundParamDescriptor m_pg;
  };

}  // namespace C15
