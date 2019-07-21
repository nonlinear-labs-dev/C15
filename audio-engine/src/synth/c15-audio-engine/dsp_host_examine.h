#pragma once

#include <stdint.h>
#include <ostream>
#include "pe_defines_config.h"
#include "pe_defines_labels.h"
#include "dsp_defines_signallabels.h"

const uint32_t tcd_log_buffersize = 1000;

const char tcd_command_names[128][4] = {
  "V",   "P",   "T",   "?83", "?84", "D",   "?86", "KU",  "U",   "?89", "?8A", "?8B", "?8C", "?8D", "?8E", "?8F",
  "VM",  "PM",  "?92", "?93", "?94", "DS",  "?96", "KD",  "UDS", "?99", "?9A", "?9B", "?9C", "?9D", "?9E", "?9F",
  "?A0", "?A1", "TU",  "?A3", "?A4", "DU",  "?A6", "RST", "?A8", "?A9", "?AA", "?AB", "?AC", "?AD", "?AE", "PL",
  "?B0", "?B1", "TL",  "?B3", "?B4", "DL",  "?B6", "KV",  "?B8", "?B9", "?BA", "?BB", "?BC", "?BD", "?BE", "?BF",
  "?C0", "?C1", "?C2", "?C3", "?C4", "?C5", "?C6", "?C7", "?C8", "?C9", "?CA", "?CB", "?CC", "?CD", "?CE", "?CF",
  "?D0", "?D1", "?D2", "?D3", "?D4", "?D5", "?D6", "?D7", "?D8", "?D9", "?DA", "?DB", "?DC", "?DD", "?DE", "?DF",
  "?E0", "?E1", "?E2", "?E3", "?E4", "?E5", "?E6", "?E7", "?E8", "?E9", "?EA", "?EB", "?EC", "?ED", "?EE", "?EF",
  "?F0", "?F1", "?F2", "?F3", "?F4", "?F5", "?F6", "?F7", "?F8", "?F9", "?FA", "?FB", "?FC", "?FD", "?FE", "?FF",
};

struct tcd_log_entry
{
  uint32_t m_time;
  uint32_t m_cmdId;
  uint32_t m_arg;

  void set(const uint32_t _time, const uint32_t _cmdId, const uint32_t _arg0, const uint32_t _arg1);
};

struct examine_tcd_input_log
{
  uint32_t m_timestamp = 0;
  uint32_t m_index = 0;
  uint32_t m_length = 0;
  uint32_t m_startPos = 0;
  tcd_log_entry m_entry[tcd_log_buffersize];

  void reset();
  void tick();
  void add(const uint32_t _cmdId, const uint32_t _arg0, const uint32_t _arg1);
};

std::ostream& operator<<(std::ostream& lhs, const examine_tcd_input_log& rhs);

struct examine_param
{
  Parameters m_selected = Parameters::P_EA_ATT;
  //
  uint32_t m_id;
  uint32_t m_index;
  uint32_t m_size;
  uint32_t m_clockType;
  uint32_t m_polyType;
  uint32_t m_scaleId;
  uint32_t m_postId;
  float m_normalize;
  float m_scaleArg;

  //
  uint32_t m_state[dsp_number_of_voices] = {};
  uint32_t m_preload[dsp_number_of_voices] = {};
  float mparams[dsp_number_of_voices] = {};
  float m_dx[dsp_number_of_voices][2] = {};
  float m_x[dsp_number_of_voices] = {};
  float m_start[dsp_number_of_voices] = {};
  float m_diff[dsp_number_of_voices] = {};
  float m_dest[dsp_number_of_voices] = {};
};

std::ostream& operator<<(std::ostream& lhs, const examine_param& rhs);

struct examine_signal
{
  Signals m_selected = Signals::ENV_A_MAG;
  uint32_t m_size = 0;
  float m_value[dsp_number_of_voices] = {};
  float m_left = 0.f;
  float m_right = 0.f;
};

std::ostream& operator<<(std::ostream& lhs, const examine_signal& rhs);
