#pragma once

/******************************************************************************/
/** @file       ae_fadepoint.h
    @date
    @version    1.7-0
    @author     M. Seeber
    @brief      fadepoint generator
    @todo
*******************************************************************************/

#include "parameter-db/generated/c15_config.h"
#include <vector>

enum class FadeEvent
{
  None,
  RecallMute,
  ToneMute,
  UnisonMute,
  MonoMute,
  Unmute  // unmute is common
};

class ae_fade_table
{
 public:
  FadeEvent m_event;
  float m_value;
  ae_fade_table();
  void init(const float _samplerate);
  bool enable(const FadeEvent _event, const uint32_t _in_or_out);
  bool get_state();
  void render();
  void stop();

 private:
  std::vector<float> m_data;
  uint32_t m_table_index, m_table_offset, m_trigger_index;
  bool m_trigger;
};

class ae_fader
{
 public:
  uint32_t m_preloaded_layerId;  // context-agnostic (mono and unison) latch for layer focus
  float m_preloaded_position;    // context-agnostic (mono and unison) latch for changed parameter position
  ae_fader();
  void init(float* _fade_table);
  void pick(const uint32_t _index);
  float get_value();
  void stop();

 private:
  float* m_data[3];
  float m_zero = 0.0f;
  float m_one = 1.0f;
  uint32_t m_index, m_current;
};
