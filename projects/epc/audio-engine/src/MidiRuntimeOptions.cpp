#include <synth/C15Synth.h>
#include "MidiRuntimeOptions.h"

void MidiRuntimeOptions::update(const nltools::msg::Setting::MidiSettingsMessage& msg)
{
  m_receiveChannel = msg.receiveChannel;
  m_receiveSplitChannel = msg.receiveSplitChannel;

  m_sendChannel = msg.sendChannel;
  m_sendSplitChannel = msg.sendSplitChannel;

  m_receiveProgramChanges = msg.receiveProgramChange;
  m_receiveNotes = msg.receiveNotes;

  m_sendProgramChanges = msg.sendProgramChange;
  m_sendNotes = msg.sendNotes;

  m_localNotes = msg.localNotes;

  pedal1CC = msg.pedal1cc;
  pedal2CC = msg.pedal2cc;
  pedal3CC = msg.pedal3cc;
  pedal4CC = msg.pedal4cc;
  ribbon1CC = msg.ribbon1cc;
  ribbon2CC = msg.ribbon2cc;
  aftertouchCC = msg.aftertouchcc;
  benderCC = msg.bendercc;

  m_enableHighVelCC = msg.highVeloCCEnabled;
  m_enable14BitCC = msg.highResCCEnabled;

  m_hwEnableMappings = msg.hwMappings;
}

MidiReceiveChannel MidiRuntimeOptions::getReceiveChannel() const
{
  return m_receiveChannel;
}

MidiSendChannel MidiRuntimeOptions::getSendChannel() const
{
  return m_sendChannel;
}

bool MidiRuntimeOptions::shouldReceiveProgramChanges() const
{
  return m_receiveProgramChanges;
}

bool MidiRuntimeOptions::shouldReceiveNotes() const
{
  return m_receiveNotes;
}

bool MidiRuntimeOptions::shouldSendProgramChanges() const
{
  return m_sendProgramChanges;
}

bool MidiRuntimeOptions::shouldSendNotes() const
{
  return m_sendNotes;
}

MidiReceiveChannelSplit MidiRuntimeOptions::getReceiveSplitChannel() const
{
  return m_receiveSplitChannel;
}

MidiSendChannelSplit MidiRuntimeOptions::getSendSplitChannel() const
{
  return m_sendSplitChannel;
}

bool MidiRuntimeOptions::shouldReceiveLocalNotes() const
{
  return m_localNotes;
}

std::optional<int> MidiRuntimeOptions::decodeEnumMSB(PedalCC cc)
{
  switch(cc)
  {
    case PedalCC::CC01:
    case PedalCC::CC02:
    case PedalCC::CC03:
    case PedalCC::CC04:
    case PedalCC::CC05:
    case PedalCC::CC06:
    case PedalCC::CC07:
    case PedalCC::CC08:
    case PedalCC::CC09:
    case PedalCC::CC10:
    case PedalCC::CC11:
    case PedalCC::CC12:
    case PedalCC::CC13:
    case PedalCC::CC14:
    case PedalCC::CC15:
    case PedalCC::CC16:
    case PedalCC::CC17:
    case PedalCC::CC18:
    case PedalCC::CC19:
    case PedalCC::CC20:
    case PedalCC::CC21:
    case PedalCC::CC22:
    case PedalCC::CC23:
    case PedalCC::CC24:
    case PedalCC::CC25:
    case PedalCC::CC26:
    case PedalCC::CC27:
    case PedalCC::CC28:
    case PedalCC::CC29:
    case PedalCC::CC30:
    case PedalCC::CC31:
      return static_cast<int>(cc);
    case PedalCC::CC64:
    case PedalCC::CC65:
    case PedalCC::CC66:
    case PedalCC::CC67:
    case PedalCC::CC68:
    case PedalCC::CC69:
      return static_cast<int>(cc) + 32;
    default:
    case PedalCC::None:
      return std::nullopt;
  }
}

std::optional<int> MidiRuntimeOptions::decodeEnumLSB(PedalCC cc)
{
  switch(cc)
  {
    case PedalCC::CC01:
    case PedalCC::CC02:
    case PedalCC::CC03:
    case PedalCC::CC04:
    case PedalCC::CC05:
    case PedalCC::CC06:
    case PedalCC::CC07:
    case PedalCC::CC08:
    case PedalCC::CC09:
    case PedalCC::CC10:
    case PedalCC::CC11:
    case PedalCC::CC12:
    case PedalCC::CC13:
    case PedalCC::CC14:
    case PedalCC::CC15:
    case PedalCC::CC16:
    case PedalCC::CC17:
    case PedalCC::CC18:
    case PedalCC::CC19:
    case PedalCC::CC20:
    case PedalCC::CC21:
    case PedalCC::CC22:
    case PedalCC::CC23:
    case PedalCC::CC24:
    case PedalCC::CC25:
    case PedalCC::CC26:
    case PedalCC::CC27:
    case PedalCC::CC28:
    case PedalCC::CC29:
    case PedalCC::CC30:
    case PedalCC::CC31:
      return static_cast<int>(cc) + 32;
    case PedalCC::CC64:
    case PedalCC::CC65:
    case PedalCC::CC66:
    case PedalCC::CC67:
    case PedalCC::CC68:
    case PedalCC::CC69:
    default:
      return std::nullopt;
  }
  return std::nullopt;
}

std::optional<int> MidiRuntimeOptions::decodeEnumMSB(RibbonCC cc)
{
  if(cc == RibbonCC::None)
    return std::nullopt;
  return static_cast<int>(cc);
}

std::optional<int> MidiRuntimeOptions::decodeEnumLSB(RibbonCC cc)
{
  if(cc == RibbonCC::None)
    return std::nullopt;
  return static_cast<int>(cc) + 32;
}

std::optional<int> MidiRuntimeOptions::decodeEnumMSB(AftertouchCC cc)
{
  using ACC = AftertouchCC;

  if(cc == ACC::PitchbendUp || cc == ACC::PitchbendDown || cc == ACC::ChannelPressure || cc == ACC::None)
    return std::nullopt;
  else
    return static_cast<int>(cc) - 1;
}

//maybe use std::optional
std::optional<int> MidiRuntimeOptions::decodeEnumLSB(AftertouchCC cc)
{
  using ACC = AftertouchCC;
  if(cc == ACC::PitchbendUp || cc == ACC::PitchbendDown || cc == ACC::ChannelPressure || cc == ACC::None)
    return std::nullopt;
  else
    return static_cast<int>(cc) + 31;
}

std::optional<int> MidiRuntimeOptions::decodeEnumMSB(BenderCC cc)
{
  if(cc == BenderCC::Pitchbend || cc == BenderCC::None)
    return std::nullopt;
  else
    return static_cast<int>(cc) - 1;
}

std::optional<int> MidiRuntimeOptions::decodeEnumLSB(BenderCC cc)
{
  if(cc == BenderCC::Pitchbend || cc == BenderCC::None)
    return std::nullopt;
  else
    return static_cast<int>(cc) + 31;
}

int MidiRuntimeOptions::channelEnumToInt(MidiSendChannel channel)
{
  if(channel == MidiSendChannel::None)
    return -1;
  else
    return static_cast<int>(channel) - 1;
}

int MidiRuntimeOptions::channelEnumToInt(MidiSendChannelSplit channel)
{
  if(channel == MidiSendChannelSplit::None || channel == MidiSendChannelSplit::Common)
    return -1;
  //Maybe return Common as -2 if we have to differentiate them

  return static_cast<int>(channel) - 1;
}

int MidiRuntimeOptions::channelEnumToInt(MidiReceiveChannel channel)
{
  if(channel == MidiReceiveChannel::None)
    return -1;
  if(channel == MidiReceiveChannel::Omni)
    return -2;  //todo reevaluate

  return static_cast<int>(channel) - 2;
}

int MidiRuntimeOptions::channelEnumToInt(MidiReceiveChannelSplit channel)
{
  if(channel == MidiReceiveChannelSplit::None || channel == MidiReceiveChannelSplit::Common)
    return -1;
  if(channel == MidiReceiveChannelSplit::Omni)
    return -2;  //todo reevaluate

  return static_cast<int>(channel) - 2;
}

void MidiRuntimeOptions::setBenderCC(BenderCC cc)
{
  benderCC = cc;
}

void MidiRuntimeOptions::setAftertouchCC(AftertouchCC cc)
{
  aftertouchCC = cc;
}

std::optional<int> MidiRuntimeOptions::getBenderMSBCC()
{
  return decodeEnumMSB(benderCC);
}

std::optional<int> MidiRuntimeOptions::getBenderLSBCC()
{
  return decodeEnumLSB(benderCC);
}

void MidiRuntimeOptions::setPedal1(PedalCC cc)
{
  pedal1CC = cc;
}

void MidiRuntimeOptions::setSendSplitChannel(MidiSendChannelSplit c)
{
  m_sendSplitChannel = c;
}

void MidiRuntimeOptions::setSendChannel(MidiSendChannel c)
{
  m_sendChannel = c;
}

void MidiRuntimeOptions::setReceiveChannel(MidiReceiveChannel c)
{
  m_receiveChannel = c;
}
void MidiRuntimeOptions::setSplitReceiveChannel(MidiReceiveChannelSplit c)
{
  m_receiveSplitChannel = c;
}

MidiReceiveChannelSplit MidiRuntimeOptions::normalToSplitChannel(MidiReceiveChannel ch)
{
  return static_cast<MidiReceiveChannelSplit>(ch);
}

std::optional<int> MidiRuntimeOptions::getAftertouchLSBCC()
{
  return decodeEnumLSB(aftertouchCC);
}

std::optional<int> MidiRuntimeOptions::getAftertouchMSBCC()
{
  return decodeEnumMSB(aftertouchCC);
}

AftertouchCC MidiRuntimeOptions::getAftertouchSetting() const
{
  return aftertouchCC;
}

bool MidiRuntimeOptions::isSwitchingCC(int pedalZeroIndexed)
{
  auto enumIsInSwitching = [](PedalCC cc) -> bool {
    switch(cc)
    {
      case PedalCC::CC64:
      case PedalCC::CC65:
      case PedalCC::CC66:
      case PedalCC::CC67:
      case PedalCC::CC68:
      case PedalCC::CC69:
        return true;
      default:
        return false;
    }
  };

  switch(pedalZeroIndexed)
  {
    case 0:
      return enumIsInSwitching(pedal1CC);
    case 1:
      return enumIsInSwitching(pedal2CC);
    case 2:
      return enumIsInSwitching(pedal3CC);
    case 3:
      return enumIsInSwitching(pedal4CC);
    default:
      return false;
  }
}

bool MidiRuntimeOptions::enableHighVelCC()
{
  return m_enableHighVelCC;
}

int MidiRuntimeOptions::getMSBCCForHWID(int hwID)
{
  auto bender = getBenderMSBCC();
  auto aftertouch = getAftertouchMSBCC();

  switch(hwID)
  {
    case 0:
      return getCCFor<Midi::MSB::Ped1>();
    case 1:
      return getCCFor<Midi::MSB::Ped2>();
    case 2:
      return getCCFor<Midi::MSB::Ped3>();
    case 3:
      return getCCFor<Midi::MSB::Ped4>();
    case 4:
      return bender.value_or(-1);
    case 5:
      return aftertouch.value_or(-1);
    case 6:
      return getCCFor<Midi::MSB::Rib1>();
    case 7:
      return getCCFor<Midi::MSB::Rib2>();
    default:
      return -1;
  }
}

BenderCC MidiRuntimeOptions::getBenderSetting() const
{
  return benderCC;
}

void MidiRuntimeOptions::setPedal2(PedalCC cc)
{
  pedal2CC = cc;
}

void MidiRuntimeOptions::setPedal3(PedalCC cc)
{
  pedal3CC = cc;
}

void MidiRuntimeOptions::setPedal4(PedalCC cc)
{
  pedal4CC = cc;
}

void MidiRuntimeOptions::setRibbon1(RibbonCC cc)
{
  ribbon1CC = cc;
}

void MidiRuntimeOptions::setRibbon2(RibbonCC cc)
{
  ribbon2CC = cc;
}

bool MidiRuntimeOptions::is14BitSupportEnabled() const
{
  return m_enable14BitCC;
}

void MidiRuntimeOptions::set14BitSupportEnabled(bool e)
{
  m_enable14BitCC = e;
}

bool MidiRuntimeOptions::shouldReceiveHWSourceOnMidiPrimary(int hwID) const
{
  constexpr auto receivePrim = static_cast<int>(tHW_ENABLE_INDICES::RECEIVE_PRIMARY);
  return m_hwEnableMappings[hwID][receivePrim];
}

bool MidiRuntimeOptions::shouldSendHWSourceOnMidiPrimary(int hwID) const
{
  constexpr auto sendPrim = static_cast<int>(tHW_ENABLE_INDICES::SEND_PRIMARY);
  return m_hwEnableMappings[hwID][sendPrim];
}

bool MidiRuntimeOptions::shouldReceiveHWSourceOnMidiSplit(int hwID) const
{
  constexpr auto receiveSplit = static_cast<int>(tHW_ENABLE_INDICES::RECEIVE_SPLIT);
  return m_hwEnableMappings[hwID][receiveSplit];
}

bool MidiRuntimeOptions::shouldSendHWSourceOnMidiSplit(int hwID) const
{
  constexpr auto sendSplit = static_cast<int>(tHW_ENABLE_INDICES::SEND_SPLIT);
  return m_hwEnableMappings[hwID][sendSplit];
}

bool MidiRuntimeOptions::shouldAllowHWSourceFromLocal(int hwID) const
{
  constexpr auto local = static_cast<int>(tHW_ENABLE_INDICES::LOCAL);
  return m_hwEnableMappings[hwID][local];
}
