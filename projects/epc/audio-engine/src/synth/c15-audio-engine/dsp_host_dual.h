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
inline constexpr bool LOG_MIDI_DETAIL = true;
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

class MidiRuntimeOptions;

using KeyShift = ShifteableKeys<C15::Config::physical_key_from, C15::Config::physical_key_to>;

class DSPInterface
{
 public:
  //remove or move somewhere else
  //TCD and MIDI should not be known to DSP
  enum class HWChangeSource
  {
    TCD,
    MIDI,
    UI
  };

  enum class InputEventSource
  {
    Internal,            //TCD -> use split
    External_Use_Split,  //Singular -> use split
    External_Primary,    //Primary Midi Channel
    External_Secondary,  //Secondary Midi Channel (UI: Split Channel)
    External_Both,       //Both   E.G Prim: CH1 + Sec: CH1
    Invalid
  };

  virtual void onHWChanged(const uint32_t id, float value, bool didBehaviourChange) = 0;
  virtual void onKeyDown(const int note, float velocity, InputEventSource from) = 0;
  virtual void onKeyUp(const int note, float velocity, InputEventSource from) = 0;
  virtual void onKeyDownSplit(const int note, float velocity, VoiceGroup part, InputEventSource from) = 0;
  virtual void onKeyUpSplit(const int note, float velocity, VoiceGroup part, InputEventSource from) = 0;
  virtual C15::Properties::HW_Return_Behavior getBehaviour(int id) = 0;
  virtual SoundType getType() = 0;
  virtual VoiceGroup getSplitPartForKeyDown(int key) = 0;
  virtual VoiceGroup getSplitPartForKeyUp(int key, InputEventSource from) = 0;
  virtual void onMidiSettingsReceived() = 0;
  static inline uint32_t getInputSourceId(const InputEventSource _inputSource)
  {
    // InputEvent can be singular (TCD or Primary) or separate (Primary or Secondary or Both)
    // Secondary can exist, so the SourceId can be 0 (TCD), 1 (Primary) or 2 (Secondary) -- Both translates to Primary
    switch(_inputSource)
    {
      case InputEventSource::Internal:
        return 0;
      case InputEventSource::External_Use_Split:
      case InputEventSource::External_Primary:
      case InputEventSource::External_Both:
        return 1;
      case InputEventSource::External_Secondary:
        return 2;
    }
    // should never be reached
    return 0;
  }
};

class dsp_host_dual : public DSPInterface
{
 public:
  void onMidiSettingsReceived() override;
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

  void onHWChanged(const uint32_t id, float value, bool didBehaviourChange) override;
  void onKeyDown(const int note, float velocity, InputEventSource from) override;
  void onKeyUp(const int note, float velocity, InputEventSource from) override;
  void onKeyDownSplit(const int note, float velocity, VoiceGroup part, InputEventSource from) override;
  void onKeyUpSplit(const int note, float velocity, VoiceGroup part, InputEventSource from) override;
  C15::Properties::HW_Return_Behavior getBehaviour(int id) override;

  // event bindings: Preset Messages
  void onPresetMessage(const nltools::msg::SinglePresetMessage& _msg);
  void onPresetMessage(const nltools::msg::SplitPresetMessage& _msg);
  void onPresetMessage(const nltools::msg::LayerPresetMessage& _msg);
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
  bool updateBehaviour(C15::ParameterDescriptor& param, ReturnMode mode);
  // evend bindings: Settings
  void onSettingEditTime(const float _position);
  void onSettingTransitionTime(const float _position);
  void onSettingGlitchSuppr(const bool _enabled);
  void onSettingTuneReference(const float _position);
  void onSettingInitialSinglePreset();
  uint32_t onSettingToneToggle();

  // dsp-related
  void render();
  void reset();

  using HWSourceValues = std::array<float, static_cast<size_t>(C15::Parameters::Hardware_Sources::_LENGTH_)>;
  HWSourceValues getHWSourceValues() const;
  SoundType getType() override;
  VoiceGroup getSplitPartForKeyDown(int key) override;
  VoiceGroup getSplitPartForKeyUp(int key, InputEventSource from) override;

  using CC_Range_7_Bit = Midi::FullCCRange<Midi::Formats::_7_Bits_>;
  using CC_Range_14_Bit = Midi::clipped14BitCCRange;
  using CC_Range_Bender = Midi::FullCCRange<Midi::Formats::_14_Bits_>;
  using CC_Range_Vel = Midi::clipped14BitVelRange;

 private:
  static inline VoiceGroup getVoiceGroupFromAllocatorId(const AllocatorId _id)
  {
    // a little inconvenient and redundant...
    switch(_id)
    {
      case AllocatorId::Local_I:
        return VoiceGroup::I;
        break;
      case AllocatorId::Local_II:
        return VoiceGroup::II;
        break;
      case AllocatorId::Local_Both:
        return VoiceGroup::Global;
        break;
    }
    // fail safety
    return VoiceGroup::NumGroups;
  }
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

  // handles for inconvenient stuff
  C15::Properties::HW_Return_Behavior getBehavior(const ReturnMode _mode);
  C15::Properties::HW_Return_Behavior getBehavior(const RibbonReturnMode _mode);
  C15::Parameters::Macro_Controls getMacro(const MacroControls _mc);
  uint32_t getMacroId(const MacroControls _mc);
  C15::Properties::LayerId getLayer(const VoiceGroup _vg);
  uint32_t getLayerId(const VoiceGroup _vg);
  // key events
  void keyDownTraversal(const uint32_t _note, const float _vel, const uint32_t _inputSourceId);
  void keyUpTraversal(const uint32_t _note, const float _vel, const uint32_t _inputSourceId);
  float scale(const Scale_Aspect _scl, float _value);
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
  friend class DspHostDualTester;
};
