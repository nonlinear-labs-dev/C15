#pragma once
#include <nltools/messaging/Message.h>
#include <variant>
#include <synth/c15-audio-engine/midi_handle.h>

class MidiRuntimeOptions
{
 public:
  [[nodiscard]] MidiReceiveChannel getReceiveChannel() const;
  [[nodiscard]] MidiReceiveChannelSplit getReceiveSplitChannel() const;
  [[nodiscard]] MidiSendChannel getSendChannel() const;
  [[nodiscard]] MidiSendChannelSplit getSendSplitChannel() const;

  [[nodiscard]] bool shouldReceiveProgramChanges() const;
  [[nodiscard]] bool shouldReceiveNotes() const;

  [[nodiscard]] bool shouldSendProgramChanges() const;
  [[nodiscard]] bool shouldSendNotes() const;

  [[nodiscard]] bool shouldReceiveLocalNotes() const;

  [[nodiscard]] bool shouldReceiveHWSourceOnMidiPrimary(int hwID) const;
  [[nodiscard]] bool shouldReceiveHWSourceOnMidiSplit(int hwID) const;
  [[nodiscard]] bool shouldSendHWSourceOnMidiPrimary(int hwID) const;
  [[nodiscard]] bool shouldSendHWSourceOnMidiSplit(int hwID) const;
  [[nodiscard]] bool shouldAllowHWSourceFromLocal(int hwID) const;

  void update(const nltools::msg::Setting::MidiSettingsMessage& msg);

  static int channelEnumToInt(MidiSendChannel channel);
  int channelEnumToInt(MidiSendChannelSplit channel);
  static int channelEnumToInt(MidiReceiveChannel channel);
  static int channelEnumToInt(MidiReceiveChannelSplit channel);
  static MidiReceiveChannelSplit normalToSplitChannel(MidiReceiveChannel ch);

  //Mapping Setters
  void setPedal1(PedalCC cc);
  void setPedal2(PedalCC cc);
  void setPedal3(PedalCC cc);
  void setPedal4(PedalCC cc);
  void setRibbon1(RibbonCC cc);
  void setRibbon2(RibbonCC cc);
  void setBenderCC(BenderCC cc);
  void setAftertouchCC(AftertouchCC cc);
  void setSendSplitChannel(MidiSendChannelSplit c);
  void setSendChannel(MidiSendChannel c);
  void setReceiveChannel(MidiReceiveChannel c);
  void setSplitReceiveChannel(MidiReceiveChannelSplit c);
  void set14BitSupportEnabled(bool e);

  template <Midi::LSB::HWSourceMidiCC tLSB> int getCCFor()
  {
    if constexpr(tLSB == Midi::LSB::Ped1)
      return decodeEnumLSB(pedal1CC).value_or(-1);
    else if constexpr(tLSB == Midi::LSB::Ped2)
      return decodeEnumLSB(pedal2CC).value_or(-1);
    else if constexpr(tLSB == Midi::LSB::Ped3)
      return decodeEnumLSB(pedal3CC).value_or(-1);
    else if constexpr(tLSB == Midi::LSB::Ped4)
      return decodeEnumLSB(pedal4CC).value_or(-1);
    else if constexpr(tLSB == Midi::LSB::Rib1)
      return decodeEnumLSB(ribbon1CC).value_or(-1);
    else if constexpr(tLSB == Midi::LSB::Rib2)
      return decodeEnumLSB(ribbon2CC).value_or(-1);
    else
      nltools_assertNotReached();
  }

  template <Midi::MSB::HWSourceMidiCC tMSB> int getCCFor()
  {
    if constexpr(tMSB == Midi::MSB::Ped1)
      return decodeEnumMSB(pedal1CC).value_or(-1);
    else if constexpr(tMSB == Midi::MSB::Ped2)
      return decodeEnumMSB(pedal2CC).value_or(-1);
    else if constexpr(tMSB == Midi::MSB::Ped3)
      return decodeEnumMSB(pedal3CC).value_or(-1);
    else if constexpr(tMSB == Midi::MSB::Ped4)
      return decodeEnumMSB(pedal4CC).value_or(-1);
    else if constexpr(tMSB == Midi::MSB::Rib1)
      return decodeEnumMSB(ribbon1CC).value_or(-1);
    else if constexpr(tMSB == Midi::MSB::Rib2)
      return decodeEnumMSB(ribbon2CC).value_or(-1);
    else if constexpr(tMSB == Midi::MSB::Aftertouch)
      return decodeEnumMSB(aftertouchCC).value_or(-1);
    else if constexpr(tMSB == Midi::MSB::Bender)
      return decodeEnumMSB(benderCC).value_or(-1);
    else
      nltools_assertNotReached();
  }

  std::optional<int> getBenderMSBCC();
  std::optional<int> getBenderLSBCC();
  std::optional<int> getAftertouchMSBCC();
  std::optional<int> getAftertouchLSBCC();
  [[nodiscard]] AftertouchCC getAftertouchSetting() const;
  [[nodiscard]] BenderCC getBenderSetting() const;

  bool isSwitchingCC(int pedalZeroIndexed);
  bool enableHighVelCC();
  bool is14BitSupportEnabled() const;
  int getMSBCCForHWID(int hwID);

 private:
  MidiReceiveChannel m_receiveChannel;
  MidiReceiveChannelSplit m_receiveSplitChannel;
  MidiSendChannel m_sendChannel;
  MidiSendChannelSplit m_sendSplitChannel;

  bool m_receiveProgramChanges = false;
  bool m_receiveNotes = false;

  bool m_sendProgramChanges = false;
  bool m_sendNotes = false;

  bool m_localNotes = false;

  bool m_enableHighVelCC = false;
  bool m_enable14BitCC = false;

  PedalCC pedal1CC;
  PedalCC pedal2CC;
  PedalCC pedal3CC;
  PedalCC pedal4CC;
  RibbonCC ribbon1CC;
  RibbonCC ribbon2CC;
  AftertouchCC aftertouchCC;
  BenderCC benderCC;

  typedef nltools::msg::Setting::MidiSettingsMessage::MAPPING_INDEX tHW_ENABLE_INDICES;
  nltools::msg::Setting::MidiSettingsMessage::tHWMappingType m_hwEnableMappings;

  //Mappings
  static std::optional<int> decodeEnumMSB(PedalCC);
  static std::optional<int> decodeEnumLSB(PedalCC);
  static std::optional<int> decodeEnumMSB(RibbonCC);
  static std::optional<int> decodeEnumLSB(RibbonCC);

  static std::optional<int> decodeEnumMSB(AftertouchCC cc);
  static std::optional<int> decodeEnumLSB(AftertouchCC cc);
  static std::optional<int> decodeEnumMSB(BenderCC cc);
  static std::optional<int> decodeEnumLSB(BenderCC cc);

  friend class MidiRuntimeOptionsTester;
};
