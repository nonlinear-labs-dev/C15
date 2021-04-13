#include <synth/C15Synth.h>
#include "MidiRuntimeOptions.h"

void MidiRuntimeOptions::update(const nltools::msg::Setting::MidiSettingsMessage& msg)
{
  m_receiveChannel = msg.receiveChannel;
  m_receiveSplitChannel = msg.receiveSplitChannel;

  m_sendChannel = msg.sendChannel;
  m_sendSplitChannel = msg.sendSplitChannel;

  m_receiveProgramChanges = msg.receiveProgramChange;
  m_receiveControllers = msg.receiveControllers;
  m_receiveNotes = msg.receiveNotes;

  m_sendProgramChanges = msg.sendProgramChange;
  m_sendControllers = msg.sendControllers;
  m_sendNotes = msg.sendNotes;

  m_localNotes = msg.localNotes;
  m_localControllers = msg.localControllers;

  pedal1CC = msg.pedal1cc;
  pedal2CC = msg.pedal2cc;
  pedal3CC = msg.pedal3cc;
  pedal4CC = msg.pedal4cc;
  ribbon1CC = msg.ribbon1cc;
  ribbon2CC = msg.ribbon2cc;
  aftertouchCC = msg.aftertouchcc;
  benderCC = msg.bendercc;

  m_enableHighVelCC = msg.highVeloCCEnabled;
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

bool MidiRuntimeOptions::shouldReceiveMIDIControllers() const
{
  return m_receiveControllers;
}

bool MidiRuntimeOptions::shouldSendProgramChanges() const
{
  return m_sendProgramChanges;
}

bool MidiRuntimeOptions::shouldSendNotes() const
{
  return m_sendNotes;
}

bool MidiRuntimeOptions::shouldSendControllers() const
{
  return m_sendControllers;
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

bool MidiRuntimeOptions::shouldReceiveLocalControllers() const
{
  return m_localControllers;
}

int MidiRuntimeOptions::ccToLSBHardwareControlID(const uint8_t i)
{
  int numCC = static_cast<int>(i);
  if(numCC == decodeEnumLSB(pedal1CC))
    return 0;
  if(numCC == decodeEnumLSB(pedal2CC))
    return 1;
  if(numCC == decodeEnumLSB(pedal3CC))
    return 2;
  if(numCC == decodeEnumLSB(pedal4CC))
    return 3;
  if(numCC == decodeEnumLSB(ribbon1CC))
    return 6;
  if(numCC == decodeEnumLSB(ribbon2CC))
    return 7;

  auto benderRes = decodeEnumLSB(benderCC);
  if(benderRes.first && numCC == benderRes.second)
    return 4;

  auto aftertouchRes = decodeEnumLSB(aftertouchCC);
  if(aftertouchRes.first && numCC == aftertouchRes.second)
    return 5;

  return -1;
}

int MidiRuntimeOptions::ccToMSBHardwareControlID(const uint8_t i)
{
  int numCC = static_cast<int>(i);
  if(numCC == decodeEnumMSB(pedal1CC))
    return 0;
  if(numCC == decodeEnumMSB(pedal2CC))
    return 1;
  if(numCC == decodeEnumMSB(pedal3CC))
    return 2;
  if(numCC == decodeEnumMSB(pedal4CC))
    return 3;
  if(numCC == decodeEnumMSB(ribbon1CC))
    return 6;
  if(numCC == decodeEnumMSB(ribbon2CC))
    return 7;

  auto benderRes = decodeEnumMSB(benderCC);
  if(benderRes.first && numCC == benderRes.second)
    return 4;

  auto aftertouchRes = decodeEnumMSB(aftertouchCC);
  if(aftertouchRes.first && numCC == aftertouchRes.second)
    return 5;

  return -1;
}

int MidiRuntimeOptions::decodeEnumMSB(PedalCC cc)
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
      return static_cast<int>(cc) + 1;
    case PedalCC::CC64:
    case PedalCC::CC65:
    case PedalCC::CC66:
    case PedalCC::CC67:
    case PedalCC::CC68:
    case PedalCC::CC69:
      return static_cast<int>(cc) + 33;
  }
  return -1;
}

int MidiRuntimeOptions::decodeEnumLSB(PedalCC cc)
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
      return static_cast<int>(cc) + 33;
    case PedalCC::CC64:
    case PedalCC::CC65:
    case PedalCC::CC66:
    case PedalCC::CC67:
    case PedalCC::CC68:
    case PedalCC::CC69:
      return -1;  //TODO fix
  }
  return -1;
}

int MidiRuntimeOptions::decodeEnumMSB(RibbonCC cc)
{
  return static_cast<int>(cc) + 1;
}

int MidiRuntimeOptions::decodeEnumLSB(RibbonCC cc)
{
  return static_cast<int>(cc) + 33;
}

std::pair<bool, int> MidiRuntimeOptions::decodeEnumMSB(AftertouchCC cc)
{
  if(cc == AftertouchCC::PitchbendUp || cc == AftertouchCC::PitchbendDown || cc == AftertouchCC::ChannelPressure)
    return { false, -1 };
  else
    return { true, static_cast<int>(cc) };
}

//maybe use std::optional
std::pair<bool, int> MidiRuntimeOptions::decodeEnumLSB(AftertouchCC cc)
{
  if(cc == AftertouchCC::PitchbendUp || cc == AftertouchCC::PitchbendDown || cc == AftertouchCC::ChannelPressure)
    return { false, -1 };
  else
    return { true, static_cast<int>(cc) + 32 };
}

std::pair<bool, int> MidiRuntimeOptions::decodeEnumMSB(BenderCC cc)
{
  if(cc == BenderCC::Pitchbend)
    return { false, -1 };
  else
    return { true, static_cast<int>(cc) };
}

std::pair<bool, int> MidiRuntimeOptions::decodeEnumLSB(BenderCC cc)
{
  if(cc == BenderCC::Pitchbend)
    return { false, -1 };
  else
    return { true, static_cast<int>(cc) + 32 };
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

std::pair<bool, int> MidiRuntimeOptions::getBenderMSBCC()
{
  return decodeEnumMSB(benderCC);
}

std::pair<bool, int> MidiRuntimeOptions::getBenderLSBCC()
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

std::pair<bool, int> MidiRuntimeOptions::getAftertouchLSBCC()
{
  return decodeEnumLSB(aftertouchCC);
}

std::pair<bool, int> MidiRuntimeOptions::getAftertouchMSBCC()
{
  return decodeEnumMSB(aftertouchCC);
}

AftertouchCC MidiRuntimeOptions::getAftertouchSetting() const
{
  return aftertouchCC;
}

bool MidiRuntimeOptions::isSwitchingCC(int pedalZeroIndexed)
{
  auto enumIsInSwitching = [](PedalCC cc) -> bool
  {
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
      return bender.second;
    case 5:
      return aftertouch.second;
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
