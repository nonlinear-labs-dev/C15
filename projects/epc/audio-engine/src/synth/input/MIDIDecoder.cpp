#include "MIDIDecoder.h"
#include <nltools/logging/Log.h>
#include <synth/c15-audio-engine/dsp_host_dual.h>
#include "MidiRuntimeOptions.h"

MIDIDecoder::MIDIDecoder(DSPInterface* dsp, MidiRuntimeOptions* options)
    : m_dsp { dsp }
    , m_options { options }
{
  reset();
}

enum class MIDIEventTypes : int
{
  Note_Off = 0,
  Note_On = 1,
  CC = 3,
  Mono_Aftertouch = 5,
  PitchBender = 6
};

bool MIDIDecoder::decode(const MidiEvent& event)
{
  const auto status = event.raw[0];
  const uint32_t type = (status & 127) >> 4;
  const auto _data0 = event.raw[1];
  const auto _data1 = event.raw[2];

  switch((MIDIEventTypes) type)
  {
    case MIDIEventTypes::Note_Off:
    {
      const uint16_t fullResVel = (_data1 << 7) + std::exchange(m_MidiLSB, 0);
      value = CC_Range_Vel::decodeUnipolarMidiValue(fullResVel);
      keyOrControl = _data0;
      m_midiChannel = statusToChannel(status);
      m_type = DecoderEventType::KeyUp;
    }
    break;
    case MIDIEventTypes::Note_On:
    {
      const uint16_t fullResVel = (_data1 << 7) + std::exchange(m_MidiLSB, 0);
      value = CC_Range_Vel::decodeUnipolarMidiValue(fullResVel);
      keyOrControl = _data0;
      m_midiChannel = statusToChannel(status);
      if(CC_Range_Vel::isValidNoteOnVelocity(fullResVel))
        m_type = DecoderEventType::KeyDown;
      else
        m_type = DecoderEventType::KeyUp;
    }
    break;
    case MIDIEventTypes::CC:
      handleIncommingCC(event);
      break;
    case MIDIEventTypes::Mono_Aftertouch:
      m_MidiLSB = 0;
      keyOrControl = 5;
      value = CC_Range_7_Bit::decodeUnipolarMidiValue(_data0);
      m_midiChannel = statusToChannel(status);
      m_type = DecoderEventType::HardwareChange;
      break;

    case MIDIEventTypes::PitchBender:
      m_MidiLSB = _data0 & 0x7F;
      keyOrControl = 4;
      value = CC_Range_Bender::decodeBipolarMidiValue((_data1 << 7) + std::exchange(m_MidiLSB, 0));
      m_midiChannel = statusToChannel(status);
      m_type = DecoderEventType::HardwareChange;
      break;

    default:
      break;
  }
  return m_type != DecoderEventType::UNKNOWN;
}

DecoderEventType MIDIDecoder::getEventType() const
{
  return m_type;
}

int MIDIDecoder::getKeyOrControl() const
{
  return keyOrControl;
}

float MIDIDecoder::getValue() const
{
  return value;
}

void MIDIDecoder::processMidiForHWSource(int id, uint32_t _data)
{
  if(m_dsp->getBehaviour(id) == C15::Properties::HW_Return_Behavior::Center)
    processBipolarMidiController<CC_Range_14_Bit>(_data, id);
  else
    processUnipolarMidiController<CC_Range_14_Bit>(_data, id);
}

template <typename Range> void MIDIDecoder::processBipolarMidiController(const uint32_t dataByte, int id)
{
  auto midiVal = (dataByte << 7) + std::exchange(m_MidiLSB, 0);
  value = Range::decodeBipolarMidiValue(midiVal);
}

template <typename Range> void MIDIDecoder::processUnipolarMidiController(const uint32_t dataByte, int id)
{
  auto midiVal = (dataByte << 7) + std::exchange(m_MidiLSB, 0);
  value = Range::decodeUnipolarMidiValue(midiVal);
}

MidiReceiveChannel MIDIDecoder::getChannel() const
{
  return m_midiChannel;
}

MidiReceiveChannel MIDIDecoder::statusToChannel(const uint8_t status)
{
  return static_cast<MidiReceiveChannel>((status & 0b00001111) + 2);
}

void MIDIDecoder::handleIncommingCC(const MidiEvent& event)
{
  const auto status = event.raw[0];
  const auto _data0 = event.raw[1];
  const auto _data1 = event.raw[2];

  auto hwSourceIDMSB = m_options->ccToMSBHardwareControlID(_data0);
  auto hwSourceIDLSB = m_options->ccToLSBHardwareControlID(_data0);

  if(hwSourceIDMSB != -1)
  {
    processMidiForHWSource(hwSourceIDMSB, _data1);
    keyOrControl = hwSourceIDMSB;
    m_midiChannel = statusToChannel(status);
    m_type = DecoderEventType::HardwareChange;
  }
  else if(hwSourceIDLSB != -1)
  {
    m_MidiLSB = _data1 & 0x7F;
  }
}

void MIDIDecoder::reset()
{
  m_MidiLSB = 0;
  m_type = DecoderEventType::UNKNOWN;
  keyOrControl = -1;
  value = 0;
  m_midiChannel = MidiReceiveChannel::None;
}
