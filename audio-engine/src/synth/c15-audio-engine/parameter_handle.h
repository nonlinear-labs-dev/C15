#pragma once

/******************************************************************************/
/** @file       parameter_handle.h
    @date
    @version    1.7-3
    @author     M. Seeber
    @brief      new main parameter handle
    @todo
*******************************************************************************/

#include "parameter_storage.h"
#include "parameter-db/generated/parameter_list.h"

namespace Engine
{

  namespace Handle
  {

    struct Clock_Handle
    {
      uint32_t m_index = 0, m_div_fast = 0, m_div_slow = 0;
      bool m_fast = false, m_slow = false;
      inline void init(const uint32_t _samplerate)
      {
        m_div_fast = _samplerate / C15::Config::clock_rates[0][1];
        m_div_slow = _samplerate / C15::Config::clock_rates[0][2];
      }
      inline void render()
      {
        m_fast = (m_index % m_div_fast) == 0;
        m_index = m_index % m_div_slow;
        m_slow = m_index == 0;
        m_index++;
      }
    };

    template <class Layer> struct Parameter_Handle
    {
      Layer_Storage m_layer[static_cast<uint32_t>(Layer::_LENGTH_)];
      Global_Storage m_global;
      const uint32_t m_layer_count = static_cast<uint32_t>(Layer::_LENGTH_);
      inline void init_modMatrix()
      {
        m_global.m_assignment.reset();
        for(uint32_t l = 0; l < m_layer_count; l++)
        {
          m_layer[l].m_assignment.reset();
        }
        // manually setting up constant return behaviors for bender (4), aftertouch (5)
        m_global.m_source[4].m_behavior = C15::Properties::HW_Return_Behavior::Center;
        m_global.m_source[5].m_behavior = C15::Properties::HW_Return_Behavior::Zero;
        for(uint32_t s = 0; s < m_global.m_source_count; s++)
        {
          m_global.m_source[s].m_offset = s * (m_global.m_macro_count - 1);
          for(uint32_t i = 1; i < m_global.m_macro_count; i++)
          {
            const uint32_t index = m_global.m_source[s].m_offset + i - 1;
            m_global.m_amount[index].m_sourceId = s;
          }
        }
      }
      inline void init_macro(const C15::ParameterDescriptor _element)
      {
        auto param = get_macro(_element.m_param.m_index);
        param->m_id = _element.m_param.m_index;
        param->m_index = _element.m_param.m_index - 1;
        param->m_position = _element.m_initial;
        param->m_initial = _element.m_initial;
      }
      inline Macro_Param* init_macro_time(const C15::ParameterDescriptor _element)
      {
        auto param = get_macro(_element.m_param.m_index);
        param->m_time.init(_element.m_ae.m_scaleId, _element.m_ae.m_scaleFactor, _element.m_ae.m_scaleOffset,
                           _element.m_initial);
        param->m_time.m_initial = _element.m_initial;
        return param;
      }
      inline void init_global_modulateable(const C15::ParameterDescriptor _element)
      {
        auto param = get_global_target(_element.m_param.m_index);
        param->m_polarity = _element.m_ae.m_polarity;
        param->m_render.m_section = _element.m_ae.m_smoother.m_section;
        param->m_render.m_clock = _element.m_ae.m_smoother.m_clock;
        param->m_render.m_index = _element.m_ae.m_smoother.m_index;
        param->m_scaling.m_id = _element.m_ae.m_scaleId;
        param->m_scaling.m_factor = _element.m_ae.m_scaleFactor;
        param->m_scaling.m_offset = _element.m_ae.m_scaleOffset;
        param->m_position = param->depolarize(_element.m_initial);
        param->m_initial = _element.m_initial;
        if(_element.m_param.m_index == static_cast<uint32_t>(C15::Parameters::Global_Modulateables::Split_Split_Point))
        {
          param->m_splitpoint = true;
        }
      }
      inline void init_global_unmodulateable(const C15::ParameterDescriptor _element)
      {
        auto param = get_global_direct(_element.m_param.m_index);
        param->m_render.m_section = _element.m_ae.m_smoother.m_section;
        param->m_render.m_clock = _element.m_ae.m_smoother.m_clock;
        param->m_render.m_index = _element.m_ae.m_smoother.m_index;
        param->m_scaling.m_id = _element.m_ae.m_scaleId;
        param->m_scaling.m_factor = _element.m_ae.m_scaleFactor;
        param->m_scaling.m_offset = _element.m_ae.m_scaleOffset;
        param->m_position = _element.m_initial;
        param->m_initial = _element.m_initial;
      }
      inline void init_local_modulateable(const C15::ParameterDescriptor _element)
      {
        for(uint32_t i = 0; i < m_layer_count; i++)
        {
          auto param = get_local_target(i, _element.m_param.m_index);
          param->m_polarity = _element.m_ae.m_polarity;
          param->m_render.m_section = _element.m_ae.m_smoother.m_section;
          param->m_render.m_clock = _element.m_ae.m_smoother.m_clock;
          param->m_render.m_index = _element.m_ae.m_smoother.m_index;
          param->m_scaling.m_id = _element.m_ae.m_scaleId;
          param->m_scaling.m_factor = _element.m_ae.m_scaleFactor;
          param->m_scaling.m_offset = _element.m_ae.m_scaleOffset;
          param->m_position = param->depolarize(_element.m_initial);
          param->m_initial = _element.m_initial;
        }
      }
      inline void init_local_unmodulateable(const C15::ParameterDescriptor _element)
      {
        for(uint32_t i = 0; i < m_layer_count; i++)
        {
          auto param = get_local_direct(i, _element.m_param.m_index);
          param->m_render.m_section = _element.m_ae.m_smoother.m_section;
          param->m_render.m_clock = _element.m_ae.m_smoother.m_clock;
          param->m_render.m_index = _element.m_ae.m_smoother.m_index;
          param->m_scaling.m_id = _element.m_ae.m_scaleId;
          param->m_scaling.m_factor = _element.m_ae.m_scaleFactor;
          param->m_scaling.m_offset = _element.m_ae.m_scaleOffset;
          param->m_position = _element.m_initial;
          param->m_initial = _element.m_initial;
        }
      }
      inline HW_Src_Param* get_hw_src(const uint32_t _id)
      {
        return &m_global.m_source[_id];
      }
      inline Direct_Param* get_global_direct(const uint32_t _id)
      {
        return &m_global.m_direct[_id];
      }
      inline Target_Param* get_global_target(const uint32_t _id)
      {
        return &m_global.m_target[_id];
      }
      inline HW_Amt_Param* get_hw_amt(const uint32_t _id)
      {
        return &m_global.m_amount[_id];
      }
      inline Macro_Param* get_macro(const uint32_t _id)
      {
        return &m_global.m_macro[_id];
      }
      inline Time_Param* get_macro_time(const uint32_t _id)
      {
        return &m_global.m_macro[_id].m_time;
      }
      inline Direct_Param* get_local_direct(const uint32_t _layerId, const uint32_t _id)
      {
        return &m_layer[_layerId].m_direct[_id];
      }
      inline Target_Param* get_local_target(const uint32_t _layerId, const uint32_t _id)
      {
        return &m_layer[_layerId].m_target[_id];
      }
      inline Target_Param* get_global_split_point()
      {
        return &m_global.m_target[static_cast<uint32_t>(C15::Parameters::Global_Modulateables::Split_Split_Point)];
      }
      inline Direct_Param* get_local_unison_voices(const Layer _layerId)
      {
        return &m_layer[static_cast<uint32_t>(_layerId)]
                    .m_direct[static_cast<uint32_t>(C15::Parameters::Local_Unmodulateables::Unison_Voices)];
      }
      inline Direct_Param* get_local_mono_enable(const Layer _layerId)
      {
        return &m_layer[static_cast<uint32_t>(_layerId)]
                    .m_direct[static_cast<uint32_t>(C15::Parameters::Local_Unmodulateables::Mono_Grp_Enable)];
      }
      inline Direct_Param* get_local_mono_priority(const Layer _layerId)
      {
        return &m_layer[static_cast<uint32_t>(_layerId)]
                    .m_direct[static_cast<uint32_t>(C15::Parameters::Local_Unmodulateables::Mono_Grp_Prio)];
      }
      inline Direct_Param* get_local_mono_legato(const Layer _layerId)
      {
        return &m_layer[static_cast<uint32_t>(_layerId)]
                    .m_direct[static_cast<uint32_t>(C15::Parameters::Local_Unmodulateables::Mono_Grp_Legato)];
      }
      inline Target_Param* globalChainFirst(const uint32_t _mcId)
      {
        return &m_global.m_target[m_global.m_assignment.first(_mcId)];
      }
      inline bool globalChainRunning()
      {
        return m_global.m_assignment.running();
      }
      inline Target_Param* globalChainNext()
      {
        return &m_global.m_target[m_global.m_assignment.next()];
      }
      inline Target_Param* localChainFirst(const uint32_t _layer, const uint32_t _mcId)
      {
        return &m_layer[_layer].m_target[m_layer[_layer].m_assignment.first(_mcId)];
      }
      inline bool localChainRunning(const uint32_t _layer)
      {
        return m_layer[_layer].m_assignment.running();
      }
      inline Target_Param* localChainNext(const uint32_t _layer)
      {
        return &m_layer[_layer].m_target[m_layer[_layer].m_assignment.next()];
      }
    };

  }  // namepsace Engine::Handle

  using Param_Handle = Handle::Parameter_Handle<C15::Properties::LayerId>;

}  // namespace Engine
