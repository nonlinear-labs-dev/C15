#include <MidiRuntimeOptions.h>

#include <utility>
#include "synth/c15-audio-engine/dsp_host_dual.h"
#include "InputEventStage.h"

InputEventStage::InputEventStage(DSPInterface *dspHost, MidiRuntimeOptions *options, HWChangedNotification hwChangedCB,
                                 InputEventStage::MIDIOut outCB)
    : m_dspHost { dspHost }
    , m_options { options }
    , m_hwChangedCB(std::move(hwChangedCB))
    , m_midiOut { std::move(outCB) }
    , m_midiDecoder(dspHost, options)
    , m_tcdDecoder(dspHost, options, &m_shifteable_keys)
{
}

void InputEventStage::onTCDMessage(const MidiEvent &tcdEvent)
{
  if(m_tcdDecoder.decode(tcdEvent))
  {
    onTCDEvent(&m_tcdDecoder);  //remove reference
    m_tcdDecoder.reset();
  }
}

void InputEventStage::onMIDIMessage(const MidiEvent &midiEvent)
{
  if(m_midiDecoder.decode(midiEvent))
  {
    onMIDIEvent(&m_midiDecoder);  //remove reference
    m_midiDecoder.reset();
  }
}

void InputEventStage::onTCDEvent(TCDDecoder *decoder)
{
  if(decoder)
  {
    VoiceGroup determinedPart = VoiceGroup::Global;
    const SoundType soundType = m_dspHost->getType();
    const bool soundValid = soundType != SoundType::Invalid;
    const auto interface = DSPInterface::InputEventSource::Internal;

    switch(decoder->getEventType())
    {
      case DecoderEventType::KeyDown:
        if(m_options->shouldReceiveLocalNotes())
        {
          if(soundType == SoundType::Split)
          {
            determinedPart = calculateSplitPartForEvent(interface, decoder->getKeyOrController());
            m_dspHost->onKeyDownSplit(decoder->getKeyOrController(), decoder->getValue(), determinedPart, interface);
          }
          else if(soundValid)
          {
            m_dspHost->onKeyDown(decoder->getKeyOrController(), decoder->getValue(), interface);
          }
        }
        if(m_options->shouldSendNotes() && soundValid)
          convertToAndSendMIDI(decoder, determinedPart);

        break;
      case DecoderEventType::KeyUp:
        if(m_options->shouldReceiveLocalNotes())
        {
          if(soundType == SoundType::Split)
          {
            determinedPart = calculateSplitPartForEvent(interface, decoder->getKeyOrController());
            m_dspHost->onKeyUpSplit(decoder->getKeyOrController(), decoder->getValue(), determinedPart, interface);
          }
          else if(soundValid)
          {
            m_dspHost->onKeyUp(decoder->getKeyOrController(), decoder->getValue(), interface);
          }
        }
        if(m_options->shouldSendNotes() && soundValid)
          convertToAndSendMIDI(decoder, determinedPart);

        break;
      case DecoderEventType::HardwareChange:
        onHWChanged(decoder->getKeyOrController(), decoder->getValue(), DSPInterface::HWChangeSource::TCD);

        break;
      case DecoderEventType::UNKNOWN:
        nltools_detailedAssertAlways(false, "Decoded Event should not have UNKNOWN Type");
    }
  }
}

void InputEventStage::onMIDIEvent(MIDIDecoder *decoder)
{
  if(decoder)
  {
    const auto soundType = m_dspHost->getType();
    const bool soundValid = soundType != SoundType::Invalid;

    switch(decoder->getEventType())
    {
      case DecoderEventType::KeyDown:
      case DecoderEventType::KeyUp:
        if(checkMIDIKeyEventEnabled(decoder))
        {
          auto inputSource = getInputSourceFromParsedChannel(decoder->getChannel());
          if(inputSource == DSPInterface::InputEventSource::Invalid)
            return;

          const auto receivedOnSecondary = inputSource == DSPInterface::InputEventSource::External_Secondary;

          if(soundType == SoundType::Split)
          {
            auto determinedPart = calculateSplitPartForEvent(inputSource, decoder->getKeyOrControl());
            if(decoder->getEventType() == DecoderEventType::KeyDown)
              m_dspHost->onKeyDownSplit(decoder->getKeyOrControl(), decoder->getValue(), determinedPart, inputSource);
            else if(decoder->getEventType() == DecoderEventType::KeyUp)
              m_dspHost->onKeyUpSplit(decoder->getKeyOrControl(), decoder->getValue(), determinedPart, inputSource);
          }
          else if(soundValid && !receivedOnSecondary)
          {
            if(decoder->getEventType() == DecoderEventType::KeyUp)
              m_dspHost->onKeyUp(decoder->getKeyOrControl(), decoder->getValue(), inputSource);
            else if(decoder->getEventType() == DecoderEventType::KeyDown)
              m_dspHost->onKeyDown(decoder->getKeyOrControl(), decoder->getValue(), inputSource);
          }
        }
        break;

      case DecoderEventType::HardwareChange:
        if(checkMIDIHardwareChangeChannelMatches(decoder))
          onMIDIHWChanged(decoder);
        break;

      case DecoderEventType::UNKNOWN:
        nltools_detailedAssertAlways(false, "Decoded Event should not have UNKNOWN Type");
    }
  }
}

void InputEventStage::convertToAndSendMIDI(TCDDecoder *pDecoder, const VoiceGroup &determinedPart)
{
  switch(pDecoder->getEventType())
  {
    case DecoderEventType::KeyDown:
      sendKeyDownAsMidi(pDecoder, determinedPart);
      break;
    case DecoderEventType::KeyUp:
      sendKeyUpAsMidi(pDecoder, determinedPart);
      break;
    case DecoderEventType::HardwareChange:
      sendHardwareChangeAsMidi(pDecoder->getKeyOrController(), pDecoder->getValue());
      break;
    case DecoderEventType::UNKNOWN:
      nltools_assertNotReached();
  }
}

bool InputEventStage::checkMIDIKeyEventEnabled(MIDIDecoder *pDecoder)
{
  using MRO = MidiRuntimeOptions;
  using MRC = MidiReceiveChannel;
  using MRCS = MidiReceiveChannelSplit;
  const auto shouldReceiveNotes = m_options->shouldReceiveNotes();
  const auto primaryChannel = m_options->getReceiveChannel();
  const auto secondaryChannel = m_options->getReceiveSplitChannel();

  const auto parsedChannel = pDecoder->getChannel();

  const auto primNotNone = primaryChannel != MRC::None;
  const auto secNotNone = secondaryChannel != MRCS::None;
  const auto primaryChannelMatches = parsedChannel == primaryChannel || primaryChannel == MRC::Omni;
  const auto secondaryChannelMatches = MRO::normalToSplitChannel(parsedChannel) == secondaryChannel
      || secondaryChannel == MRCS::Omni || secondaryChannel == MRCS::Common;
  const auto channelMatches = primaryChannelMatches || secondaryChannelMatches;
  return shouldReceiveNotes && channelMatches && (primNotNone || secNotNone);
}

bool InputEventStage::checkMIDIHardwareChangeChannelMatches(MIDIDecoder *pDecoder)
{
  const auto channelMatches = m_options->getReceiveChannel() == MidiReceiveChannel::Omni
      || pDecoder->getChannel() == m_options->getReceiveChannel()
      || MidiRuntimeOptions::normalToSplitChannel(pDecoder->getChannel()) == m_options->getReceiveSplitChannel()
      || m_options->getReceiveSplitChannel() == MidiReceiveChannelSplit::Omni;
  return channelMatches;
}

void InputEventStage::sendKeyDownAsMidi(TCDDecoder *pDecoder, const VoiceGroup &determinedPart)
{
  const auto mainChannel = MidiRuntimeOptions::channelEnumToInt(m_options->getSendChannel());
  const auto secondaryChannel = m_options->channelEnumToInt(m_options->getSendSplitChannel());
  const auto key = pDecoder->getKeyOrController();
  constexpr const uint8_t keyType = 0x90;
  constexpr const uint8_t ccType = 0xB0;
  const uint8_t keyByte = static_cast<uint8_t>(key) & 0x7F;

  if(mainChannel != -1
     && ((determinedPart == VoiceGroup::I || determinedPart == VoiceGroup::Global)
         || m_options->getSendSplitChannel() == MidiSendChannelSplit::Common))
  {
    auto mainC = static_cast<uint8_t>(mainChannel);
    const uint8_t keyStatus = keyType | mainC;

    if(m_options->enableHighVelCC())
    {
      uint16_t fullResolutionValue = CC_Range_Vel::encodeUnipolarMidiValue(pDecoder->getValue());

      const uint8_t lsbStatus = ccType | mainC;
      auto lsbVelByte = static_cast<uint8_t>(fullResolutionValue & 0x7F);
      m_midiOut({ lsbStatus, 88, lsbVelByte });

      auto msbVelByte = static_cast<uint8_t>(fullResolutionValue >> 7);
      m_midiOut({ keyStatus, keyByte, msbVelByte });
    }
    else
    {
      auto stdVal = static_cast<uint8_t>(CC_Range_7_Bit::encodeUnipolarMidiValue(pDecoder->getValue()));
      m_midiOut({ keyStatus, keyByte, stdVal });
    }

    if(m_options->getSendSplitChannel() == MidiSendChannelSplit::Common)
      return;
  }

  if(secondaryChannel != -1 && (determinedPart == VoiceGroup::II || determinedPart == VoiceGroup::Global)
     && m_dspHost->getType() == SoundType::Split)
  {
    auto secC = static_cast<uint8_t>(secondaryChannel);
    const uint8_t keyStatus = keyType | secC;

    if(m_options->enableHighVelCC())
    {
      uint16_t fullResolutionValue = CC_Range_Vel::encodeUnipolarMidiValue(pDecoder->getValue());

      const uint8_t lsbStatus = ccType | secC;
      auto lsbVelByte = static_cast<uint8_t>(fullResolutionValue & 0x7F);
      m_midiOut({ lsbStatus, 88, lsbVelByte });

      auto msbVelByte = static_cast<uint8_t>(fullResolutionValue >> 7);
      m_midiOut({ keyStatus, keyByte, msbVelByte });
    }
    else
    {
      auto stdVal = static_cast<uint8_t>(CC_Range_7_Bit::encodeUnipolarMidiValue(pDecoder->getValue()));
      m_midiOut({ keyStatus, keyByte, stdVal });
    }
  }
}

void InputEventStage::sendKeyUpAsMidi(TCDDecoder *pDecoder, const VoiceGroup &determinedPart)
{
  const auto mainChannel = MidiRuntimeOptions::channelEnumToInt(m_options->getSendChannel());
  const auto secondaryChannel = m_options->channelEnumToInt(m_options->getSendSplitChannel());
  const auto key = pDecoder->getKeyOrController();

  const auto mainC = static_cast<uint8_t>(mainChannel);
  const auto secC = static_cast<uint8_t>(secondaryChannel);
  constexpr const uint8_t keyType = 0x80;
  constexpr const uint8_t ccType = 0xB0;

  if(mainChannel != -1
     && ((determinedPart == VoiceGroup::I || determinedPart == VoiceGroup::Global)
         || m_options->getSendSplitChannel() == MidiSendChannelSplit::Common))
  {
    const uint8_t keyStatus = keyType | mainC;
    uint8_t keyByte = static_cast<uint8_t>(key) & 0x7F;

    if(m_options->enableHighVelCC())
    {
      const uint8_t lsbStatus = ccType | mainC;
      uint16_t fullResolutionValue = CC_Range_Vel::encodeUnipolarMidiValue(pDecoder->getValue());
      auto lsbVelByte = static_cast<uint8_t>(fullResolutionValue & 0x7F);
      m_midiOut({ lsbStatus, 88, lsbVelByte });

      auto msbVelByte = static_cast<uint8_t>(fullResolutionValue >> 7);
      m_midiOut({ keyStatus, keyByte, msbVelByte });
    }
    else
    {
      auto stdValue = static_cast<uint8_t>(CC_Range_7_Bit::encodeUnipolarMidiValue(pDecoder->getValue()));
      m_midiOut({ keyStatus, keyByte, stdValue });
    }

    if(m_options->getSendSplitChannel() == MidiSendChannelSplit::Common)
      return;
  }

  if(secondaryChannel != -1 && (determinedPart == VoiceGroup::II || determinedPart == VoiceGroup::Global)
     && m_dspHost->getType() == SoundType::Split)
  {
    const uint8_t keyStatus = keyType | secC;
    uint8_t keyByte = static_cast<uint8_t>(key) & 0x7F;

    if(m_options->enableHighVelCC())
    {
      const uint8_t lsbStatus = ccType | secC;
      uint16_t fullResolutionValue = CC_Range_Vel::encodeUnipolarMidiValue(pDecoder->getValue());
      auto lsbVelByte = static_cast<uint8_t>(fullResolutionValue & 0x7F);
      m_midiOut({ lsbStatus, 88, lsbVelByte });

      auto msbVelByte = static_cast<uint8_t>(fullResolutionValue >> 7);
      m_midiOut({ keyStatus, keyByte, msbVelByte });
    }
    else
    {
      auto stdValue = static_cast<uint8_t>(CC_Range_7_Bit::encodeUnipolarMidiValue(pDecoder->getValue()));
      m_midiOut({ keyStatus, keyByte, stdValue });
    }
  }
}

void InputEventStage::sendHardwareChangeAsMidi(int hwID, float value)
{
  auto roundPedalToSwitching = [](float val) -> float
  {
    if(val >= 0.5f)
      return 1.0f;
    else
      return 0.0f;
  };

  switch(hwID)
  {
    case 0:
      if(m_options->isSwitchingCC(0))
        value = roundPedalToSwitching(value);
      sendCCOut(0, value, m_options->getCCFor<Midi::MSB::Ped1>(), m_options->getCCFor<Midi::LSB::Ped1>());
      break;

    case 1:
      if(m_options->isSwitchingCC(1))
        value = roundPedalToSwitching(value);
      sendCCOut(1, value, m_options->getCCFor<Midi::MSB::Ped2>(), m_options->getCCFor<Midi::LSB::Ped2>());
      break;

    case 2:
      if(m_options->isSwitchingCC(2))
        value = roundPedalToSwitching(value);
      sendCCOut(2, value, m_options->getCCFor<Midi::MSB::Ped3>(), m_options->getCCFor<Midi::LSB::Ped3>());
      break;

    case 3:
      if(m_options->isSwitchingCC(3))
        value = roundPedalToSwitching(value);
      sendCCOut(3, value, m_options->getCCFor<Midi::MSB::Ped4>(), m_options->getCCFor<Midi::LSB::Ped4>());
      break;

    case 4:
      doSendBenderOut(value);
      break;

    case 5:
      doSendAftertouchOut(value);
      break;

    case 6:
      sendCCOut(6, value, m_options->getCCFor<Midi::MSB::Rib1>(), m_options->getCCFor<Midi::LSB::Rib1>());
      break;

    case 7:
      sendCCOut(7, value, m_options->getCCFor<Midi::MSB::Rib2>(), m_options->getCCFor<Midi::LSB::Rib2>());
      break;

    default:
      break;
  }
}

void InputEventStage::sendCCOut(int hwID, float value, std::optional<int> msbCC, std::optional<int> lsbCC)
{
  using CC_Range_14_Bit = Midi::clipped14BitCCRange;

  if(m_dspHost->getBehaviour(hwID) == C15::Properties::HW_Return_Behavior::Center)
    doSendCCOut(CC_Range_14_Bit::encodeBipolarMidiValue(value), msbCC, lsbCC);
  else
    doSendCCOut(CC_Range_14_Bit::encodeUnipolarMidiValue(value), msbCC, lsbCC);
}

void InputEventStage::doSendCCOut(uint16_t value, std::optional<int> msbCC, std::optional<int> lsbCC)
{
  const auto mainChannel = MidiRuntimeOptions::channelEnumToInt(m_options->getSendChannel());
  const auto secondaryChannel = m_options->channelEnumToInt(m_options->getSendSplitChannel());

  const auto mainC = static_cast<uint8_t>(mainChannel);
  const auto secC = static_cast<uint8_t>(secondaryChannel);

  auto statusByte = static_cast<uint8_t>(0xB0);
  auto lsbValByte = static_cast<uint8_t>(value & 0x7F);
  auto msbValByte = static_cast<uint8_t>(value >> 7 & 0x7F);

  if(mainChannel != -1)
  {
    auto mainStatus = static_cast<uint8_t>(statusByte | mainC);

    if(lsbCC.has_value() && m_options->is14BitSupportEnabled())
    {
      m_midiOut({ mainStatus, static_cast<uint8_t>(lsbCC.value()), lsbValByte });
    }

    if(msbCC.has_value())
    {
      m_midiOut({ mainStatus, static_cast<uint8_t>(msbCC.value()), msbValByte });
    }
  }

  if(secondaryChannel != -1)
  {
    auto secStatus = static_cast<uint8_t>(statusByte | secC);
    if(lsbCC.has_value() && m_options->is14BitSupportEnabled())
    {
      m_midiOut({ secStatus, static_cast<uint8_t>(lsbCC.value()), lsbValByte });
    }

    if(msbCC.has_value())
    {
      m_midiOut({ secStatus, static_cast<uint8_t>(msbCC.value()), msbValByte });
    }
  }
}

void InputEventStage::setNoteShift(int i)
{
  m_shifteable_keys.setNoteShift(i);
}

VoiceGroup InputEventStage::calculateSplitPartForEvent(DSPInterface::InputEventSource inputEvent, const int keyNumber)
{
  switch(inputEvent)
  {
    case DSPInterface::InputEventSource::Internal:
    case DSPInterface::InputEventSource::External_Use_Split:
      return m_dspHost->getSplitPartForKey(keyNumber);
    case DSPInterface::InputEventSource::External_Primary:
      return VoiceGroup::I;
    case DSPInterface::InputEventSource::External_Secondary:
      return VoiceGroup::II;
    case DSPInterface::InputEventSource::External_Both:
      return VoiceGroup::Global;
    case DSPInterface::InputEventSource::Invalid:
      break;
  }
  nltools_assertNotReached();
}

DSPInterface::InputEventSource InputEventStage::getInputSourceFromParsedChannel(MidiReceiveChannel channel)
{
  const auto primary = m_options->getReceiveChannel();
  const auto secondary = m_options->getReceiveSplitChannel();
  const auto primaryMask = midiReceiveChannelMask(primary);
  const auto secondaryMask = midiReceiveChannelMask(secondary);
  // first check: did event qualify at all?
  const bool qualifiedForEvent = (primaryMask | secondaryMask) > 0;
  if(!qualifiedForEvent)
    return DSPInterface::InputEventSource::Invalid;
  // second check: obtain channel mask and evaluate common "singular" case
  const auto channelMask = midiReceiveChannelMask(channel);
  const bool qualifiedForCommon = secondary == MidiReceiveChannelSplit::Common;
  const bool qualifiedForPrimary = (channelMask & primaryMask) > 0;
  if(qualifiedForCommon)
    return qualifiedForPrimary ? DSPInterface::InputEventSource::External_Use_Split
                               : DSPInterface::InputEventSource::Invalid;

  // third checks: evaluate "separate" cases (both, primary, secondary)
  const bool qualifiedForSecondary = (channelMask & secondaryMask) > 0;
  const bool qualifiedForIdentity = qualifiedForPrimary && qualifiedForSecondary;
  if(qualifiedForIdentity)
    return DSPInterface::InputEventSource::External_Both;
  if(qualifiedForPrimary)
    return DSPInterface::InputEventSource::External_Primary;
  if(qualifiedForSecondary)
    return DSPInterface::InputEventSource::External_Secondary;
  // this line should never be reached
  return DSPInterface::InputEventSource::Invalid;
}

void InputEventStage::doSendAftertouchOut(float value)
{
  using CC_Range_14_Bit = Midi::clipped14BitCCRange;

  if(m_options->getAftertouchSetting() == AftertouchCC::None)
  {
    return;
  }
  else if(m_options->getAftertouchSetting() == AftertouchCC::ChannelPressure)
  {
    const auto mainChannel = MidiRuntimeOptions::channelEnumToInt(m_options->getSendChannel());
    const auto secondaryChannel = m_options->channelEnumToInt(m_options->getSendSplitChannel());

    const auto mainC = static_cast<uint8_t>(mainChannel);
    const auto secC = static_cast<uint8_t>(secondaryChannel);

    auto atStatusByte = static_cast<uint8_t>(0xD0);
    uint8_t valByte = CC_Range_7_Bit::encodeUnipolarMidiValue(value);

    if(mainChannel != -1)
    {
      auto mainStatus = static_cast<uint8_t>(atStatusByte | mainC);
      m_midiOut({ mainStatus, valByte });
    }

    if(secondaryChannel != -1)
    {
      auto secStatus = static_cast<uint8_t>(atStatusByte | secC);
      m_midiOut({ secStatus, valByte });
    }
  }
  else if(m_options->getAftertouchSetting() == AftertouchCC::PitchbendDown
          || m_options->getAftertouchSetting() == AftertouchCC::PitchbendUp)
  {
    if(m_options->getAftertouchSetting() == AftertouchCC::PitchbendDown)
      value *= -1.f;

    using CC_Range_Bender = Midi::FullCCRange<Midi::Formats::_14_Bits_>;

    auto v = CC_Range_Bender::encodeBipolarMidiValue(value);
    auto lsb = static_cast<uint8_t>(v & 0x7F);
    auto msb = static_cast<uint8_t>((v >> 7) & 0x7F);

    const auto mainChannel = MidiRuntimeOptions::channelEnumToInt(m_options->getSendChannel());
    const auto secondaryChannel = m_options->channelEnumToInt(m_options->getSendSplitChannel());

    const auto mainC = static_cast<uint8_t>(mainChannel);
    const auto secC = static_cast<uint8_t>(secondaryChannel);

    auto statusByte = static_cast<uint8_t>(0xE0);

    if(mainChannel != -1)
    {
      auto mainStatus = static_cast<uint8_t>(statusByte | mainC);
      m_midiOut({ mainStatus, lsb, msb });
    }

    if(secondaryChannel != -1)
    {
      auto secStatus = static_cast<uint8_t>(statusByte | secC);
      m_midiOut({ secStatus, lsb, msb });
    }
  }
  else
  {
    auto atLSB = m_options->getAftertouchLSBCC();
    auto atMSB = m_options->getAftertouchMSBCC();
    const auto hasBothCCs = atLSB.has_value() && atMSB.has_value();

    if(hasBothCCs)
    {
      doSendCCOut(CC_Range_14_Bit::encodeUnipolarMidiValue(value), atMSB.value(), atLSB.value());
    }
  }
}

void InputEventStage::doSendBenderOut(float value)
{
  const auto isMappedToNone = m_options->getBenderSetting() == BenderCC::None;
  if(isMappedToNone)
    return;

  if(m_options->getBenderSetting() == BenderCC::Pitchbend)
  {
    using CC_Range_Bender = Midi::FullCCRange<Midi::Formats::_14_Bits_>;

    auto v = CC_Range_Bender::encodeBipolarMidiValue(value);
    auto lsb = static_cast<uint8_t>(v & 0x7F);
    auto msb = static_cast<uint8_t>((v >> 7) & 0x7F);

    const auto mainChannel = MidiRuntimeOptions::channelEnumToInt(m_options->getSendChannel());
    const auto secondaryChannel = m_options->channelEnumToInt(m_options->getSendSplitChannel());

    const auto mainC = static_cast<uint8_t>(mainChannel);
    const auto secC = static_cast<uint8_t>(secondaryChannel);

    auto statusByte = static_cast<uint8_t>(0xE0);

    if(mainChannel != -1)
    {
      auto mainStatus = static_cast<uint8_t>(statusByte | mainC);
      m_midiOut({ mainStatus, lsb, msb });
    }

    if(secondaryChannel != -1)
    {
      auto secStatus = static_cast<uint8_t>(statusByte | secC);
      m_midiOut({ secStatus, lsb, msb });
    }
  }
  else
  {
    auto benderLSB = m_options->getBenderLSBCC();
    auto benderMSB = m_options->getBenderMSBCC();
    const auto hasCCs = benderLSB.has_value() && benderMSB.has_value();

    if(hasCCs)
    {
      using CC_Range_14_Bit = Midi::clipped14BitCCRange;
      auto lsbCC = benderLSB.value();
      auto msbCC = benderMSB.value();
      doSendCCOut(CC_Range_14_Bit::encodeBipolarMidiValue(value), msbCC, lsbCC);
    }
  }
}

void InputEventStage::onUIHWSourceMessage(const nltools::msg::HWSourceChangedMessage &message)
{
  auto hwID = InputEventStage::parameterIDToHWID(message.parameterId);

  if(hwID.has_value())
  {
    onHWChanged(hwID.value(), message.controlPosition, DSPInterface::HWChangeSource::UI);
  }
}

std::optional<int> InputEventStage::parameterIDToHWID(int id)
{
  switch(id)
  {
    case C15::PID::Pedal_1:
      return 0;
    case C15::PID::Pedal_2:
      return 1;
    case C15::PID::Pedal_3:
      return 2;
    case C15::PID::Pedal_4:
      return 3;
    case C15::PID::Bender:
      return 4;
    case C15::PID::Aftertouch:
      return 5;
    case C15::PID::Ribbon_1:
      return 6;
    case C15::PID::Ribbon_2:
      return 7;
    default:
      return std::nullopt;
  }
}

void InputEventStage::onHWChanged(int hwID, float pos, DSPInterface::HWChangeSource source)
{

  auto sendToDSP = [&](auto source)
  {
    switch(source)
    {
      case DSPInterface::HWChangeSource::MIDI:
        return m_options->shouldReceiveMIDIControllers();
      case DSPInterface::HWChangeSource::TCD:
        return m_options->shouldReceiveLocalControllers();
      case DSPInterface::HWChangeSource::UI:
        return true;
      default:
        nltools_assertNotReached();
    }
  };

  if(sendToDSP(source))
  {
    m_dspHost->onHWChanged(hwID, pos);
    m_hwChangedCB();
  }

  if(m_options->shouldSendControllers() && source != DSPInterface::HWChangeSource::MIDI)
  {
    if(filterUnchangedHWPositions(hwID, pos))
    {
      sendHardwareChangeAsMidi(hwID, pos);
    }
  }
}

float processMidiForHWSource(DSPInterface *dsp, int id, uint8_t msb, uint8_t lsb)
{
  using CC_Range_14_Bit = Midi::clipped14BitCCRange;
  auto midiVal = (static_cast<uint16_t>(msb) << 7) + lsb;

  if(dsp->getBehaviour(id) == C15::Properties::HW_Return_Behavior::Center)
    return CC_Range_14_Bit::decodeBipolarMidiValue(midiVal);
  else
    return CC_Range_14_Bit::decodeUnipolarMidiValue(midiVal);
}

void InputEventStage::onMIDIHWChanged(MIDIDecoder *decoder)
{
  if(m_options->shouldReceiveMIDIControllers())
  {
    auto hwRes = decoder->getHWChangeStruct();

    for(auto hwID = 0; hwID < 8; hwID++)
    {
      const auto mappedMSBCC = m_options->getMSBCCForHWID(hwID);
      if(mappedMSBCC.has_value() && mappedMSBCC.value() == hwRes.receivedCC)
      {
        const auto msb = hwRes.undecodedValueBytes[0];
        const auto lsb = hwRes.undecodedValueBytes[1];
        float realVal = processMidiForHWSource(m_dspHost, hwID, msb, lsb);
        m_dspHost->onHWChanged(hwID, realVal);
        m_hwChangedCB();
      }
      else
      {
        if(hwID == 4 && hwRes.cases == MIDIDecoder::MidiHWChangeSpecialCases::ChannelPitchbend)
        {
          if(m_options->getBenderSetting() == BenderCC::Pitchbend)
          {
            onHWChanged(4, decoder->getValue(), DSPInterface::HWChangeSource::MIDI);
          }
        }

        if(hwID == 5)
        {
          if(hwRes.cases == MIDIDecoder::MidiHWChangeSpecialCases::Aftertouch)
          {
            if(m_options->getAftertouchSetting() == AftertouchCC::ChannelPressure)
            {
              onHWChanged(5, decoder->getValue(), DSPInterface::HWChangeSource::MIDI);
            }
          }

          if(hwRes.cases == MIDIDecoder::MidiHWChangeSpecialCases::ChannelPitchbend)
          {
            auto pitchbendValue = decoder->getValue();

            if(m_options->getAftertouchSetting() == AftertouchCC::PitchbendUp)
            {
              pitchbendValue = std::max(0.0f, pitchbendValue);
              onHWChanged(5, pitchbendValue, DSPInterface::HWChangeSource::MIDI);
            }
            else if(m_options->getAftertouchSetting() == AftertouchCC::PitchbendDown)
            {
              pitchbendValue = -std::min(0.0f, pitchbendValue);
              onHWChanged(5, pitchbendValue, DSPInterface::HWChangeSource::MIDI);
            }
          }
        }
      }
    }
  }
}

bool InputEventStage::filterUnchangedHWPositions(int id, float pos)
{
  if(m_latchedHWPositions[id] != pos)
  {
    m_latchedHWPositions[id] = pos;
    return true;
  }
  return false;
}

constexpr uint16_t InputEventStage::midiReceiveChannelMask(const MidiReceiveChannel &_channel)
{
  return c_midiReceiveMaskTable[static_cast<uint8_t>(_channel)];
}

constexpr uint16_t InputEventStage::midiReceiveChannelMask(const MidiReceiveChannelSplit &_channel)
{
  return c_midiReceiveMaskTable[static_cast<uint8_t>(_channel)];
}
