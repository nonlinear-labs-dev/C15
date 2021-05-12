#pragma once

#include <memory>
#include <Types.h>
#include <synth/input/TCDDecoder.h>
#include <synth/input/MIDIDecoder.h>
#include <functional>
#include <nltools/messaging/Message.h>
#include <synth/c15-audio-engine/dsp_host_dual.h>

class MidiRuntimeOptions;

class InputEventStage
{
 public:
  using MIDIOutType = nltools::msg::Midi::SimpleMessage;
  using MIDIOut = std::function<void(MIDIOutType)>;

  //use reference
  InputEventStage(DSPInterface* dspHost, MidiRuntimeOptions* options, MIDIOut outCB);
  void onTCDMessage(const MidiEvent& tcdEvent);
  void onMIDIMessage(const MidiEvent& midiEvent);
  void onUIHWSourceMessage(const nltools::msg::HWSourceChangedMessage& message);
  void setNoteShift(int i);

 private:
  using CC_Range_Vel = Midi::clipped14BitVelRange;
  using CC_Range_7_Bit = Midi::FullCCRange<Midi::Formats::_7_Bits_>;

  //TCD/MIDI In
  bool checkMIDIKeyEventEnabled(MIDIDecoder* pDecoder);
  bool checkMIDIHardwareChangeChannelMatches(MIDIDecoder* pDecoder);
  void onMIDIEvent(MIDIDecoder* decoder);
  void onTCDEvent(TCDDecoder* decoder);
  void onMIDIHWChanged(MIDIDecoder* decoder);

  //Algorithm
  void onHWChanged(int hwID, float pos, DSPInterface::HWChangeSource source);
  VoiceGroup calculateSplitPartForEvent(DSPInterface::InputEventSource inputEvent, const int keyNumber);
  DSPInterface::InputEventSource getInputSourceFromParsedChannel(MidiReceiveChannel channel);
  bool filterUnchangedHWPositions(int id, float pos);

  static constexpr uint16_t midiReceiveChannelMask(const MidiReceiveChannel& _channel);
  static constexpr uint16_t midiReceiveChannelMask(const MidiReceiveChannelSplit& _channel);
  static int parameterIDToHWID(int id);
  static int HWIDToParameterID(int id);

  //MIDI and UI out
  void convertToAndSendMIDI(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void sendKeyDownAsMidi(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void sendKeyUpAsMidi(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void sendHardwareChangeAsMidi(int hwID, float value);
  void doSendAftertouchOut(float value);
  void doSendBenderOut(float value);

  void sendCCOut(int hwID, float value, int msbCC, int lsbCC);
  void doSendCCOut(uint16_t value, int msbCC, int lsbCC);

  static constexpr uint16_t c_midiReceiveMaskTable[19] = {
    0x0000,  // None (no bit is set)
    0xFFFF,  // Omni (all bits are set)
    0x0001,  // CH_1 (specific channels 0..15 refer to specific bits 0..15)
    0x0002,  // CH_2
    0x0004,  // CH_3
    0x0008,  // CH_4
    0x0010,  // CH_5
    0x0020,  // CH_6
    0x0040,  // CH_7
    0x0080,  // CH_8
    0x0100,  // CH_9
    0x0200,  // CH_10
    0x0400,  // CH_11
    0x0800,  // CH_12
    0x1000,  // CH_13
    0x2000,  // CH_14
    0x4000,  // CH_15
    0x8000,  // CH_16
    0x0000   // Common = None
  };

  TCDDecoder m_tcdDecoder;
  MIDIDecoder m_midiDecoder;
  DSPInterface* m_dspHost;
  MidiRuntimeOptions* m_options;
  MIDIOut m_midiOut;
  KeyShift m_shifteable_keys;
  std::array<float, 8> m_latchedHWPositions { std::numeric_limits<float>::max() };

  friend class InputEventStageTester;
};

//namespace InputStateDetail
//{
//  using Event = DSPInterface::InputEvent;
//  using State = DSPInterface::InputState;
//  using Source = DSPInterface::InputSource;
//
//  static constexpr Event Unknown = { Source::Unknown, State::Invalid };
//  static constexpr Event TCD = { Source::TCD, State::Singular };
//  static constexpr Event Singular = { Source::Primary, State::Singular };
//  static constexpr Event Primary = { Source::Primary, State::Separate };
//  static constexpr Event Both = { Source::Both, State::Separate };
//  static constexpr Event Secondary = { Source::Secondary, State::Separate };
//}
