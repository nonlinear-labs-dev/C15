#pragma once

/******************************************************************************/
/** @file       dsp_host_dual.h
    @date
    @version    1.7-3
    @author     M. Seeber
    @brief      new main engine container
    @todo
*******************************************************************************/

#include <c15_config.h>
#include <nltools/messaging/Message.h>
#include <parameter_list.h>

#include "parameter_handle.h"
#include "pe_exponentiator.h"
#include "voice_allocation.h"

#include "ae_fadepoint.h"
#include "ae_global_section.h"
#include "ae_mono_section.h"
#include "ae_poly_section.h"

#include "midi_handle.h"

#include <array>

// developer switches
inline constexpr unsigned int SAMPLE_COUNT = 0;  // 0: normal, 1: acceptance-test: does host really render?

// addendum - voice fade: enable proposed interpolated approach for generating fade values
inline constexpr bool VOICE_FADE_INTERPOLATION = true;

// basic logging switches
inline constexpr bool LOG_MISSING = false;
inline constexpr bool LOG_FAIL = false;
inline constexpr bool LOG_INIT = false;
inline constexpr bool LOG_MIDI_TCD = false;
inline constexpr bool LOG_MIDI_RAW = false;
inline constexpr bool LOG_MIDI_DETAIL = false;
inline constexpr bool LOG_MIDI_OUT = false;
inline constexpr bool LOG_DISPATCH = false;
inline constexpr bool LOG_EDITS = false;
inline constexpr bool LOG_TIMES = false;
inline constexpr bool LOG_SETTINGS = false;
inline constexpr bool LOG_RECALL = false;
inline constexpr bool LOG_RECALL_DETAILS = false;
inline constexpr bool LOG_RECALL_LEVELS = false;
inline constexpr bool LOG_KEYS = false;
inline constexpr bool LOG_KEYS_POLY = false;
inline constexpr bool LOG_TRANSITIONS = false;
inline constexpr bool LOG_RESET = false;
inline constexpr bool LOG_HW = false;
// more detailed logging of specific parameters
inline constexpr bool LOG_ENGINE_STATUS = false;
inline constexpr bool LOG_ENGINE_EDITS = false;
inline constexpr uint32_t LOG_PARAMS_LENGTH = 3;
// use tcd ids here (currently: Split Point, Unison Detune)
static const uint32_t LOG_PARAMS[LOG_PARAMS_LENGTH] = { 356, 250, 367 };

class dsp_host_dual
{
 public:
  // public members
  float m_mainOut_R = 0.0f, m_mainOut_L = 0.0f;
  uint32_t m_sample_counter = 0;
  // constructor
  dsp_host_dual();
  // public methods
  void init(const uint32_t _samplerate, const uint32_t _polyphony);
  // handles for inconvenient stuff
  C15::ParameterDescriptor getParameter(const int _id);
  // event bindings: debug
  void logStatus();
  // event bindings: Playcontroller or MIDI Device (in Dev_PC mode)

  using SimpleRawMidiMessage = nltools::msg::Midi::SimpleMessage;

  using MidiOut = std::function<void(const SimpleRawMidiMessage&)>;

  void onTcdMessage(const uint32_t _status, const uint32_t _data0, const uint32_t _data1,
                    const MidiOut& out = getNullMidiOut());

  void onMidiMessage(const uint32_t _status, const uint32_t _data0, const uint32_t _data1);
  // event bindings: Preset Messages
  void onPresetMessage(const nltools::msg::SinglePresetMessage& _msg);
  void onPresetMessage(const nltools::msg::SplitPresetMessage& _msg);
  void onPresetMessage(const nltools::msg::LayerPresetMessage& _msg);
  // event bindings: Parameter Changed Messages
  void globalParChg(const uint32_t _id, const nltools::msg::HWSourceChangedMessage& _msg);
  void globalParChg(const uint32_t _id, const nltools::msg::HWAmountChangedMessage& _msg);
  void globalParChg(const uint32_t _id, const nltools::msg::MacroControlChangedMessage& _msg);
  void globalParChg(const uint32_t _id, const nltools::msg::ModulateableParameterChangedMessage& _msg);
  void globalParChg(const uint32_t _id, const nltools::msg::UnmodulateableParameterChangedMessage& _msg);
  void globalTimeChg(const uint32_t _id, const nltools::msg::UnmodulateableParameterChangedMessage& _msg);
  void localParChg(const uint32_t _id, const nltools::msg::ModulateableParameterChangedMessage& _msg);
  void localParChg(const uint32_t _id, const nltools::msg::UnmodulateableParameterChangedMessage& _msg);
  void localUnisonVoicesChg(const nltools::msg::UnmodulateableParameterChangedMessage& _msg);
  void localMonoEnableChg(const nltools::msg::UnmodulateableParameterChangedMessage& _msg);
  void localMonoPriorityChg(const nltools::msg::UnmodulateableParameterChangedMessage& _msg);
  void localMonoLegatoChg(const nltools::msg::UnmodulateableParameterChangedMessage& _msg);
  // evend bindings: Settings
  void onSettingEditTime(const float _position);
  void onSettingTransitionTime(const float _position);
  void onSettingNoteShift(const int& _shift);
  void onSettingGlitchSuppr(const bool _enabled);
  void onSettingTuneReference(const float _position);
  void onSettingInitialSinglePreset();
  uint32_t onSettingToneToggle();
  // dsp-related
  void render();
  void reset();

  using HWSourceValues = std::array<float, static_cast<size_t>(C15::Parameters::Hardware_Sources::_LENGTH_)>;
  HWSourceValues getHWSourceValues() const;
  void hwSourceToMidi(const uint32_t id, const float controlPosition, const MidiOut& out);

  using CC_Range_7_Bit = Midi::FullCCRange<Midi::Formats::_7_Bits_>;
  using CC_Range_14_Bit = Midi::clipped14BitCCRange;
  using CC_Range_Bender = Midi::FullCCRange<Midi::Formats::_14_Bits_>;
  using CC_Range_Vel = Midi::clipped14BitVelRange;

 private:
  using LayerMode = C15::Properties::LayerMode;
  // parameters
  Engine::Param_Handle m_params;
  Time_Param m_edit_time, m_transition_time;
  Setting_Param m_reference;
  const C15::ParameterDescriptor m_invalid_param = { C15::None };
  // essential tools
  exponentiator m_convert;
  Engine::Handle::Clock_Handle m_clock;
  Engine::Handle::Time_Handle m_time;
  // layer handling
  LayerMode m_layer_mode;
  // global dsp components
  GlobalSection m_global;
  VoiceAllocation<C15::Config::total_polyphony, C15::Config::local_polyphony, C15::Config::virtual_key_count,
                  C15::Config::generic_key_pivot, LayerMode>
      m_alloc;
  ShifteableKeys<C15::Config::physical_key_from, C15::Config::physical_key_to> m_shifteable_keys;
  // dsp components
  atomic_fade_table m_fade;
  PolySection m_poly[2];
  MonoSection m_mono[2];
  LayerSignalCollection m_z_layers[2];
  // helper values
  const float m_format_vel = 16383.0f / 127.0f, m_format_hw = 16000.0f / 127.0f, m_format_pb = 16000.0f / 16383.0f,
              m_norm_vel = 1.0f / 16383.0f, m_norm_hw = 1.0f / 16000.0f;
  int32_t m_key_pos = 0;
  uint32_t m_tone_state = 0;
  bool m_glitch_suppression = false;

  std::array<uint8_t, 8> m_hwSourcesMidiLSB;
  uint8_t m_velocityLSB = 0;

  // handles for inconvenient stuff
  C15::Properties::HW_Return_Behavior getBehavior(const ReturnMode _mode);
  C15::Properties::HW_Return_Behavior getBehavior(const RibbonReturnMode _mode);
  C15::Parameters::Macro_Controls getMacro(const MacroControls _mc);
  uint32_t getMacroId(const MacroControls _mc);
  C15::Properties::LayerId getLayer(const VoiceGroup _vg);
  uint32_t getLayerId(const VoiceGroup _vg);
  // key events
  void keyDown(const float _vel);
  void keyUp(const float _vel);
  float scale(const Scale_Aspect _scl, float _value);
  // inner event flow
  void updateHW(const uint32_t _id, const float _raw, const MidiOut& out);
  void updateTime(Time_Aspect* _param, const float _ms);
  void hwModChain(HW_Src_Param* _src, const uint32_t _id, const float _inc);
  void globalModChain(Macro_Param* _mc);
  void localModChain(Macro_Param* _mc);
  void localModChain(const uint32_t _layer, Macro_Param* _mc);
  void globalTransition(const Target_Param* _param, const Time_Aspect _time);
  void globalTransition(const Direct_Param* _param, const Time_Aspect _time);
  void localTransition(const uint32_t _layer, const Direct_Param* _param, const Time_Aspect _time);
  void localTransition(const uint32_t _layer, const Target_Param* _param, const Time_Aspect _time);

  bool evalPolyChg(const C15::Properties::LayerId _layerId,
                   const nltools::msg::ParameterGroups::UnmodulateableParameter& _unisonVoices,
                   const nltools::msg::ParameterGroups::UnmodulateableParameter& _monoEnable);
  void evalVoiceFadeChg(const uint32_t _layer);
  void recallSingle(const nltools::msg::SinglePresetMessage& _msg);
  void recallSplit(const nltools::msg::SplitPresetMessage& _msg);
  void recallLayer(const nltools::msg::LayerPresetMessage& _msg);
  void globalParRcl(const nltools::msg::ParameterGroups::HardwareSourceParameter& _param);
  void globalParRcl(const nltools::msg::ParameterGroups::HardwareAmountParameter& _param);
  void globalParRcl(const nltools::msg::ParameterGroups::MacroParameter& _param);
  void globalParRcl(const nltools::msg::ParameterGroups::ModulateableParameter& _param);
  void globalParRcl(const nltools::msg::ParameterGroups::UnmodulateableParameter& _param);
  void globalParRcl(const nltools::msg::ParameterGroups::GlobalParameter& _param);
  void globalTimeRcl(const nltools::msg::ParameterGroups::UnmodulateableParameter& _param);
  void localParRcl(const uint32_t _layerId, const nltools::msg::ParameterGroups::ModulateableParameter& _param);
  void localParRcl(const uint32_t _layerId, const nltools::msg::ParameterGroups::SplitPoint& _param);
  void localParRcl(const uint32_t _layerId, const nltools::msg::ParameterGroups::UnmodulateableParameter& _param);
  void localPolyRcl(const uint32_t _layerId, const bool _va_update,
                    const nltools::msg::ParameterGroups::UnisonGroup& _unison,
                    const nltools::msg::ParameterGroups::MonoGroup& _mono);
  void debugLevels();

  static inline MidiOut getNullMidiOut()
  {
    return [](const SimpleRawMidiMessage&) {};
  }

  template <typename Range> void processBipolarMidiController(const uint32_t dataByte, int id);
  template <typename Range> void processUnipolarMidiController(const uint32_t dataByte, int id);
  void processNormalizedMidiController(const uint32_t _id, const float _controlPosition);
  template <Midi::MSB::HWSourceMidiCC msb, Midi::LSB::HWSourceMidiCC lsb>
  void sendCCOut(int id, float controlPosition, const MidiOut& out);
  void processMidiForHWSource(int id, uint32_t _data);
};
