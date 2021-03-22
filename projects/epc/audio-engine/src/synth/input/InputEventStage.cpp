#include <MidiRuntimeOptions.h>

#include <utility>
#include "synth/c15-audio-engine/dsp_host_dual.h"
#include "InputEventStage.h"

InputEventStage::InputEventStage(DSPInterface *dspHost, MidiRuntimeOptions *options, InputEventStage::MIDIOut outCB)
    : m_dspHost{ dspHost }
    , m_options{ options }
    , m_midiOut{ std::move(outCB) }
    , m_midiDecoder(dspHost, options)
    , m_tcdDecoder(dspHost, options, &m_shifteable_keys)
{
}

void InputEventStage::onTCDMessage(const MidiEvent &tcdEvent)
{
  if(m_tcdDecoder.decode(tcdEvent))
  {
    onTCDEvent(&m_tcdDecoder);
    m_tcdDecoder.reset();
  }
}

void InputEventStage::onMIDIMessage(const MidiEvent &midiEvent)
{
  if(m_midiDecoder.decode(midiEvent))
  {
    onMIDIEvent(&m_midiDecoder);
    m_midiDecoder.reset();
  }
}

void InputEventStage::onTCDEvent(TCDDecoder *decoder)
{
  if(decoder)
  {
    // it would be benefitial/desirable to not call calculatePartForEvent() multiple times within a single event chain,
    // therefore the following suggestion: we can determine the part once within the stack and pass it subsequently
    VoiceGroup determinedPart = VoiceGroup::Global;    // initially, we set it to global
    const SoundType soundType = m_dspHost->getType();  // also, we track the sound type for more safety
    const bool soundValid = soundType != SoundType::Invalid;
    switch(decoder->getEventType())
    {
      case DecoderEventType::KeyDown:
        if(m_options->shouldReceiveLocalNotes())
        {
          if(soundType == SoundType::Split)
          {
            determinedPart = calculatePartForEvent(decoder);  // Split Sound overrides part association for key
            m_dspHost->onKeyDownSplit(decoder->getKeyOrController(), decoder->getValue(), determinedPart,
                                      DSPInterface::InputSource::TCD);
          }
          else if(soundValid)  // safety first
          {
            // Single/Layer Sound acts global (maybe, only primary is preferred?)
            m_dspHost->onKeyDown(decoder->getKeyOrController(), decoder->getValue(), DSPInterface::InputSource::TCD);
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
            determinedPart = calculatePartForEvent(decoder);  // Split Sound overrides part association for key
            m_dspHost->onKeyUpSplit(decoder->getKeyOrController(), decoder->getValue(), determinedPart,
                                    DSPInterface::InputSource::TCD);
          }
          else if(soundValid)  // safety first
          {
            // Single/Layer Sound acts global (maybe, only primary is preferred?)
            m_dspHost->onKeyUp(decoder->getKeyOrController(), decoder->getValue(), DSPInterface::InputSource::TCD);
          }
        }
        if(m_options->shouldSendNotes() && soundValid)  // safety first
          convertToAndSendMIDI(decoder, determinedPart);

        break;
      case DecoderEventType::HardwareChange:
        if(m_options->shouldReceiveLocalControllers())
          m_dspHost->onHWChanged(decoder->getKeyOrController(), decoder->getValue());
        if(m_options->shouldSendControllers())
          // HW Sources seem to act without part association?
          convertToAndSendMIDI(decoder, determinedPart);

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
    // it would be benefitial/desirable to not call calculatePartForEvent() multiple times within a single event chain,
    // therefore the following suggestion: we can determine the part once within the stack and pass it subsequently
    VoiceGroup determinedPart = VoiceGroup::Global;    // initially, we set it to global
    const SoundType soundType = m_dspHost->getType();  // also, we track the sound type for more safety
    const bool soundValid = soundType != SoundType::Invalid;
    switch(decoder->getEventType())
    {
      case DecoderEventType::KeyDown:
        if(checkMIDIKeyDownEnabled(decoder))
        {
          if(soundType == SoundType::Split)
          {
            determinedPart = calculatePartForEvent(decoder);  // Split Sound overrides part association for key
            m_dspHost->onKeyDownSplit(decoder->getKeyOrControl(), decoder->getValue(), determinedPart,
                                      getInterfaceFromDecoder(decoder));
          }
          else if(soundValid)  // safety first
          {
            m_dspHost->onKeyDown(decoder->getKeyOrControl(), decoder->getValue(), getInterfaceFromDecoder(decoder));
          }
        }
        break;

      case DecoderEventType::KeyUp:
        if(checkMIDIKeyUpEnabled(decoder))
        {
          if(soundType == SoundType::Split)
          {
            determinedPart = calculatePartForEvent(decoder);  // Split Sound overrides part association for key
            m_dspHost->onKeyUpSplit(decoder->getKeyOrControl(), decoder->getValue(), determinedPart,
                                    getInterfaceFromDecoder(decoder));
          }
          else if(soundValid)  // safety first
          {
            m_dspHost->onKeyUp(decoder->getKeyOrControl(), decoder->getValue(), getInterfaceFromDecoder(decoder));
          }
        }
        break;

      case DecoderEventType::HardwareChange:
        if(checkMIDIHardwareChangeEnabled(decoder))
          m_dspHost->onHWChanged(decoder->getKeyOrControl(), decoder->getValue());
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
      sendHardwareChangeAsMidi(pDecoder);
      // TODO: what about primary/secondary ctrl chg? (likewise: prog chg)
      break;
    case DecoderEventType::UNKNOWN:
      nltools_assertNotReached();
  }
}

bool InputEventStage::checkMIDIKeyDownEnabled(MIDIDecoder *pDecoder)
{
  using MRC = MidiReceiveChannel;
  const auto shouldReceiveNotes = m_options->shouldReceiveNotes();
  const auto primaryChannel = m_options->getReceiveChannel();
  const auto secondaryChannel = m_options->splitToNormalChannel(m_options->getReceiveSplitChannel());

  const auto parsedChannel = pDecoder->getChannel();

  const auto primaryChannelMatches = parsedChannel == primaryChannel || primaryChannel == MRC::Omni;
  const auto secondaryChannelMatches = parsedChannel == secondaryChannel || secondaryChannel == MRC::Omni;
  const auto channelMatches = primaryChannelMatches || secondaryChannelMatches;
  return shouldReceiveNotes && channelMatches;
}

bool InputEventStage::checkMIDIKeyUpEnabled(MIDIDecoder *pDecoder)
{
  return checkMIDIKeyDownEnabled(pDecoder);
}

bool InputEventStage::checkMIDIHardwareChangeEnabled(MIDIDecoder *pDecoder)
{
  const auto recControls = m_options->shouldReceiveControllers();
  const auto channelMatches = pDecoder->getChannel() == m_options->getReceiveChannel()
      || m_options->getReceiveChannel() == MidiReceiveChannel::Omni;
  return recControls && channelMatches;
}

void InputEventStage::sendKeyDownAsMidi(TCDDecoder *pDecoder, const VoiceGroup &determinedPart)
{
  using CC_Range_Vel = Midi::clipped14BitVelRange;

  const auto mainChannel = MidiRuntimeOptions::channelEnumToInt(m_options->getSendChannel());
  const auto secondaryChannel = m_options->channelEnumToInt(m_options->getSendSplitChannel());
  //  const auto keyPart = m_dspHost->getSplitPartForKey(pDecoder->getKeyOrController());
  const auto key = pDecoder->getKeyOrController();

  if(mainChannel != -1 && (determinedPart == VoiceGroup::I || determinedPart == VoiceGroup::Global))
  {
    auto mainC = static_cast<uint8_t>(mainChannel);

    constexpr const uint8_t ccType = 0xB0;
    const uint8_t lsbStatus = ccType | mainC;

    uint16_t fullResolutionValue = CC_Range_Vel::encodeUnipolarMidiValue(pDecoder->getValue());
    auto lsbVelByte = static_cast<uint8_t>(fullResolutionValue & 0x7F);
    m_midiOut({ lsbStatus, 88, lsbVelByte });

    constexpr const uint8_t keyType = 0x90;
    const uint8_t keyStatus = keyType | mainC;
    uint8_t keyByte = static_cast<uint8_t>(key) & 0x7F;
    auto msbVelByte = static_cast<uint8_t>(fullResolutionValue >> 7);
    m_midiOut({ keyStatus, keyByte, msbVelByte });
  }

  if(secondaryChannel != -1 && (determinedPart == VoiceGroup::II || determinedPart == VoiceGroup::Global))
  {
    auto secC = static_cast<uint8_t>(secondaryChannel);

    constexpr const uint8_t ccType = 0xB0;
    const uint8_t lsbStatus = ccType | secC;

    uint16_t fullResolutionValue = CC_Range_Vel::encodeUnipolarMidiValue(pDecoder->getValue());
    auto lsbVelByte = static_cast<uint8_t>(fullResolutionValue & 0x7F);
    m_midiOut({ lsbStatus, 88, lsbVelByte });

    constexpr const uint8_t keyType = 0x90;
    const uint8_t keyStatus = keyType | secC;
    uint8_t keyByte = static_cast<uint8_t>(key) & 0x7F;
    auto msbVelByte = static_cast<uint8_t>(fullResolutionValue >> 7);
    m_midiOut({ keyStatus, keyByte, msbVelByte });
  }
}

void InputEventStage::sendKeyUpAsMidi(TCDDecoder *pDecoder, const VoiceGroup &determinedPart)
{
  // key up events should include part evaluation (as key down events)
  using CC_Range_Vel = Midi::clipped14BitVelRange;

  uint16_t fullResolutionValue = CC_Range_Vel::encodeUnipolarMidiValue(pDecoder->getValue());
  auto lsbVelByte = static_cast<uint8_t>(fullResolutionValue & 0x7F);
  m_midiOut({ 0xB0, 88, lsbVelByte });

  uint8_t keyByte = static_cast<uint8_t>(pDecoder->getKeyOrController()) & 0x7F;
  auto msbVelByte = static_cast<uint8_t>(fullResolutionValue >> 7);
  m_midiOut({ 0x80, keyByte, msbVelByte });
}

void InputEventStage::sendHardwareChangeAsMidi(TCDDecoder *pDecoder)
{
  using CC_Range_7_Bit = Midi::FullCCRange<Midi::Formats::_7_Bits_>;
  using CC_Range_Bender = Midi::FullCCRange<Midi::Formats::_14_Bits_>;

  switch(pDecoder->getKeyOrController())
  {
    case 0:
      sendCCOut(0, pDecoder->getValue(), m_options->getCCFor<Midi::MSB::Ped1>(),
                m_options->getCCFor<Midi::LSB::Ped1>());
      break;

    case 1:
      sendCCOut(1, pDecoder->getValue(), m_options->getCCFor<Midi::MSB::Ped2>(),
                m_options->getCCFor<Midi::LSB::Ped2>());
      break;

    case 2:
      sendCCOut(2, pDecoder->getValue(), m_options->getCCFor<Midi::MSB::Ped3>(),
                m_options->getCCFor<Midi::LSB::Ped3>());
      break;

    case 3:
      sendCCOut(3, pDecoder->getValue(), m_options->getCCFor<Midi::MSB::Ped4>(),
                m_options->getCCFor<Midi::LSB::Ped4>());
      break;

    case 6:
      sendCCOut(6, pDecoder->getValue(), m_options->getCCFor<Midi::MSB::Rib1>(),
                m_options->getCCFor<Midi::LSB::Rib1>());
      break;

    case 7:
      sendCCOut(7, pDecoder->getValue(), m_options->getCCFor<Midi::MSB::Rib2>(),
                m_options->getCCFor<Midi::LSB::Rib2>());
      break;

    case 4:  //Bender
    {
      //TODO add bender SEND
      auto value = CC_Range_Bender::encodeBipolarMidiValue(pDecoder->getValue());
      auto statusByte = static_cast<uint8_t>(0xE0);
      auto valByte1 = static_cast<uint8_t>(value & 0x7F);
      auto valByte2 = static_cast<uint8_t>((value >> 7) & 0x7F);
      // Pitch Bend Messages are: Status LSB MSB
      m_midiOut({ statusByte, valByte1, valByte2 });
      break;
    }

    case 5:  //Aftertouch
    {
      //TODO add Aftertouch SEND
      auto statusByte = static_cast<uint8_t>(0xD0);
      uint8_t valByte = CC_Range_7_Bit::encodeUnipolarMidiValue(pDecoder->getValue());
      m_midiOut({ statusByte, valByte });
      break;
    }

    default:
      break;
  }
}
void InputEventStage::sendCCOut(int hwID, float value, int msbCC, int lsbCC)
{
  using CC_Range_14_Bit = Midi::clipped14BitCCRange;

  if(m_dspHost->getBehaviour(hwID) == C15::Properties::HW_Return_Behavior::Center)
    doSendCCOut(CC_Range_14_Bit::encodeBipolarMidiValue(value), msbCC, lsbCC);
  else
    doSendCCOut(CC_Range_14_Bit::encodeUnipolarMidiValue(value), msbCC, lsbCC);
}

void InputEventStage::doSendCCOut(uint16_t value, int msbCC, int lsbCC)
{
  auto statusByte = static_cast<uint8_t>(0xB0);
  auto lsbValByte = static_cast<uint8_t>(value & 0x7F);
  m_midiOut({ statusByte, static_cast<uint8_t>(lsbCC), lsbValByte });

  auto msbValByte = static_cast<uint8_t>(value >> 7 & 0x7F);
  m_midiOut({ statusByte, static_cast<uint8_t>(msbCC), msbValByte });
}

void InputEventStage::setNoteShift(int i)
{
  m_shifteable_keys.setNoteShift(i);
}

VoiceGroup InputEventStage::calculatePartForEvent(MIDIDecoder *pDecoder)
{
  //if split && (sec != prim && sec != none && prim != none) {
  //pDec->getChannel == prim ? Part::I
  //pDec->getChannel == sec ? Part::II
  //} else {
  // getPartForSplitKey(pDec->getKeyOrControl());
  //}

  //?A?! TODO
  const auto primChannel = m_options->getReceiveChannel();
  const auto secChannel = m_options->splitToNormalChannel(m_options->getReceiveSplitChannel());

  const auto primIsOmni = primChannel == MidiReceiveChannel::Omni;
  const auto secIsOmni = secChannel == MidiReceiveChannel::Omni;

  switch(pDecoder->getEventType())
  {
    case DecoderEventType::KeyUp:
    case DecoderEventType::KeyDown:
      if(primChannel == secChannel && (primChannel == pDecoder->getChannel() || primIsOmni || secIsOmni))
        return VoiceGroup::I;  //?? TODO FIX
      if(primChannel == pDecoder->getChannel() || primIsOmni)
        return VoiceGroup::I;
      else if(secChannel == pDecoder->getChannel() || secIsOmni)
        return VoiceGroup::II;
      break;
    default:
    case DecoderEventType::HardwareChange:
      break;
  }

  return VoiceGroup::Global;
}

VoiceGroup InputEventStage::calculatePartForEvent(TCDDecoder *pDecoder)
{
  return m_dspHost->getSplitPartForKey(pDecoder->getKeyOrController());
}

DSPInterface::InputSource InputEventStage::getInterfaceFromDecoder(MIDIDecoder *pDecoder)
{
  const auto primChannel = m_options->getReceiveChannel();
  const auto secChannel = m_options->splitToNormalChannel(m_options->getReceiveSplitChannel());
  const auto areDifferent = primChannel != secChannel;
  const auto primNotNone = primChannel != MidiReceiveChannel::None;
  const auto secNotNone = secChannel != MidiReceiveChannel::None;

  if(areDifferent)
  {
    if(primNotNone)
    {
      if(pDecoder->getChannel() == primChannel)
        return DSPInterface::InputSource::Primary;
    }

    if(secNotNone)
    {
      if(pDecoder->getChannel() == secChannel)
        return DSPInterface::InputSource::Secondary;
    }
  }

  return DSPInterface::InputSource::Unknown;
}
