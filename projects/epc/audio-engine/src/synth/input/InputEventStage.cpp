#include <MidiRuntimeOptions.h>

#include <utility>
#include "synth/c15-audio-engine/dsp_host_dual.h"
#include "InputEventStage.h"

InputEventStage::InputEventStage(DSPInterface *dspHost, MidiRuntimeOptions *options, InputEventStage::MIDIOut outCB)
    : m_dspHost { dspHost }
    , m_options { options }
    , m_midiOut { std::move(outCB) }
    , m_midiDecoder(dspHost, options)
    , m_tcdDecoder(dspHost, options)
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
    switch(decoder->getEventType())
    {
      case DecoderEventType::KeyDown:
        if(m_options->shouldReceiveLocalNotes())
          m_dspHost->onKeyDown(decoder->getKeyOrController(), decoder->getValue(), VoiceGroup::I);
        if(m_options->shouldSendNotes())
          convertToAndSendMIDI(decoder);

        break;
      case DecoderEventType::KeyUp:
        if(m_options->shouldReceiveLocalNotes())
          m_dspHost->onKeyUp(decoder->getKeyOrController(), decoder->getValue(), VoiceGroup::I);
        if(m_options->shouldSendNotes())
          convertToAndSendMIDI(decoder);

        break;
      case DecoderEventType::HardwareChange:
        if(m_options->shouldReceiveLocalControllers())
          m_dspHost->onHWChanged(decoder->getKeyOrController(), decoder->getValue());
        if(m_options->shouldSendControllers())
          convertToAndSendMIDI(decoder);

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
    switch(decoder->getEventType())
    {
      case DecoderEventType::KeyDown:
        if(checkMIDIKeyDownEnabled(decoder))
          m_dspHost->onKeyDown(decoder->getKeyOrControl(), decoder->getValue(), VoiceGroup::I);
        break;

      case DecoderEventType::KeyUp:
        if(checkMIDIKeyUpEnabled(decoder))
          m_dspHost->onKeyUp(decoder->getKeyOrControl(), decoder->getValue(), VoiceGroup::I);
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

void InputEventStage::convertToAndSendMIDI(TCDDecoder *pDecoder)
{
  switch(pDecoder->getEventType())
  {
    case DecoderEventType::KeyDown:
      sendKeyDownAsMidi(pDecoder);
      break;
    case DecoderEventType::KeyUp:
      sendKeyUpAsMidi(pDecoder);
      break;
    case DecoderEventType::HardwareChange:
      sendHardwareChangeAsMidi(pDecoder);
      break;
    case DecoderEventType::UNKNOWN:
      nltools_assertNotReached();
  }
}

bool InputEventStage::checkMIDIKeyDownEnabled(MIDIDecoder *pDecoder)
{
  const auto recNotes = m_options->shouldReceiveNotes();
  const auto channelMatches = pDecoder->getChannel() == m_options->getReceiveChannel()
      || m_options->getReceiveChannel() == MidiReceiveChannel::Omni;
  return recNotes && channelMatches;
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

void InputEventStage::sendKeyDownAsMidi(TCDDecoder *pDecoder)
{
  using CC_Range_Vel = Midi::clipped14BitVelRange;

  uint16_t fullResolutionValue = CC_Range_Vel::encodeUnipolarMidiValue(pDecoder->getValue());
  auto lsbVelByte = static_cast<uint8_t>(fullResolutionValue & 0x7F);
  m_midiOut({ 0xB0, 88, lsbVelByte });

  uint8_t keyByte = static_cast<uint8_t>(pDecoder->getKeyOrController()) & 0x7F;
  auto msbVelByte = static_cast<uint8_t>(fullResolutionValue >> 7);
  m_midiOut({ 0x90, keyByte, msbVelByte });
}

void InputEventStage::sendKeyUpAsMidi(TCDDecoder *pDecoder)
{
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
