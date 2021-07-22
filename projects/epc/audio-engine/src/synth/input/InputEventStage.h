#pragma once

#include <memory>
#include <Types.h>
#include <synth/input/TCDDecoder.h>
#include <synth/input/MIDIDecoder.h>
#include <functional>
#include <nltools/messaging/Message.h>
#include <synth/c15-audio-engine/dsp_host_dual.h>
#include "MidiChannelModeMessages.h"

class MidiRuntimeOptions;

namespace HWID
{
  constexpr static auto PEDAL1 = 0;
  constexpr static auto PEDAL2 = 1;
  constexpr static auto PEDAL3 = 2;
  constexpr static auto PEDAL4 = 3;
  constexpr static auto BENDER = 4;
  constexpr static auto AFTERTOUCH = 5;
  constexpr static auto RIBBON1 = 6;
  constexpr static auto RIBBON2 = 7;

  constexpr static auto INVALID = -1;
};

class InputEventStage
{
 public:
  using RoutingIndex = nltools::msg::Setting::MidiSettingsMessage::RoutingIndex;
  using MIDIOutType = nltools::msg::Midi::SimpleMessage;
  using MIDIOut = std::function<void(MIDIOutType)>;
  using HWChangedNotification = std::function<void()>;
  using ChannelModeMessageCB = std::function<void(MidiChannelModeMessages)>;

  //use reference
  InputEventStage(DSPInterface* dspHost, MidiRuntimeOptions* options, HWChangedNotification hwChangedCB, MIDIOut outCB,
                  ChannelModeMessageCB specialFunctionOut);
  void onTCDMessage(const MidiEvent& tcdEvent);
  void onMIDIMessage(const MidiEvent& midiEvent);
  void onUIHWSourceMessage(const nltools::msg::HWSourceChangedMessage& message, bool didBehaviourChange);
  void setNoteShift(int i);

  static int parameterIDToHWID(int id);

  bool getAndResetKeyBedStatus();

 private:
  void setAndScheduleKeybedNotify();

  using CC_Range_Vel = Midi::clipped14BitVelRange;
  using CC_Range_7_Bit = Midi::FullCCRange<Midi::Formats::_7_Bits_>;

  //TCD/MIDI In
  bool checkMIDIKeyEventEnabled(MIDIDecoder* pDecoder);
  bool checkMIDIHardwareChangeChannelMatches(MIDIDecoder* pDecoder);
  void onMIDIEvent();
  void onTCDEvent();
  void onMIDIHWChanged(MIDIDecoder* decoder);

  //Algorithm
  void onHWChanged(int hwID, float pos, DSPInterface::HWChangeSource source, bool wasMIDIPrimary, bool wasMIDISplit,
                   bool didBehaviourChange);

  VoiceGroup calculateSplitPartForKeyDown(DSPInterface::InputEventSource inputEvent, const int keyNumber);
  VoiceGroup calculateSplitPartForKeyUp(DSPInterface::InputEventSource inputEvent, const int keyNumber);
  DSPInterface::InputEventSource getInputSourceFromParsedChannel(MidiReceiveChannel channel);

  static constexpr uint16_t midiReceiveChannelMask(const MidiReceiveChannel& _channel);
  static constexpr uint16_t midiReceiveChannelMask(const MidiReceiveChannelSplit& _channel);

  //MIDI and UI out
  void convertToAndSendMIDI(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void sendKeyDownAsMidi(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void sendKeyUpAsMidi(TCDDecoder* pDecoder, const VoiceGroup& determinedPart);
  void sendHardwareChangeAsMidi(int hwID, float value);
  void doSendAftertouchOut(float value);
  void doSendBenderOut(float value);

  void sendCCOut(int hwID, float value, int msbCC, int lsbCC);
  void doSendCCOut(uint16_t value, int msbCC, int lsbCC, int hwID);

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
  HWChangedNotification m_hwChangedCB;
  ChannelModeMessageCB m_channelModeMessageCB;
  MIDIOut m_midiOut;
  KeyShift m_shifteable_keys;
  std::array<std::array<uint16_t, 2>, 8> m_latchedHWPositions;
  bool m_notifyKeyBedActionStatus = false;

  enum class LatchMode
  {
    Option,
    LSBAndMSB,
    OnlyMSB
  };

  template <LatchMode> bool latchHWPosition(int hwID, uint8_t lsb, uint8_t msb);
  [[nodiscard]] bool isSplitDSP() const;

  friend class InputEventStageTester;
  bool ccIsMappedToChannelModeMessage(int cc);
  void queueChannelModeMessage(int cc, uint8_t msbCCvalue);
};
