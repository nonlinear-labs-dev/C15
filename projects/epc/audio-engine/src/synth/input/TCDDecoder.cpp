#include <synth/C15Synth.h>
#include "TCDDecoder.h"

TCDDecoder::TCDDecoder(DSPInterface *dsp, MidiRuntimeOptions *options, KeyShift *keyShift)
    : m_dsp { dsp }
    , m_options { options }
    , m_keyShift { keyShift }
{
  reset();
}

bool TCDDecoder::decode(const MidiEvent &event)
{
  const auto _status = event.raw[0];
  const auto _data0 = event.raw[1];
  const auto _data1 = event.raw[2];
  const uint32_t channel = _status & 0b00001111;
  const uint32_t st = (_status & 0b01111111) >> 4;

  if(st == 6)
  {
    if(channel >= 0 && channel <= 7)
    {
      uint32_t arg = _data1 + (_data0 << 7);
      value = static_cast<float>(arg) * c_norm_hw;  // HW src normalization by 1 / 16000

      const auto behaviour = m_dsp->getBehaviour(channel);

      if(behaviour == C15::Properties::HW_Return_Behavior::Center)
      {
        value = (2.0f * value) - 1.0f;
      }

      if constexpr(LOG_MIDI_TCD)
      {
        nltools::Log::warning("Got HW-Src:", channel, "value:", value, "behaviour", static_cast<int>(behaviour), "raw data0", (short)_data0, "raw data1",(short)_data1);
      }

      keyOrController = channel;
      m_type = DecoderEventType::HardwareChange;
    }
    else if(channel == 13)  //Key Pos
    {
      keyOrController = _data1;
    }
    else if(channel == 14)  //Key Down
    {
      uint32_t arg = _data1 + (_data0 << 7);
      keyOrController = m_keyShift->keyDown(keyOrController);
      if((keyOrController >= C15::Config::virtual_key_from) && (keyOrController <= C15::Config::virtual_key_to))
      {
        value = static_cast<float>(arg) * c_norm_vel;  // VEL normalization by 1 / 16383
        m_type = DecoderEventType::KeyDown;
      }
    }
    else if(channel == 15)  //Key Up
    {
      uint32_t arg = _data1 + (_data0 << 7);
      keyOrController = m_keyShift->keyUp(keyOrController);
      if((keyOrController >= C15::Config::virtual_key_from) && (keyOrController <= C15::Config::virtual_key_to))
      {
        value = static_cast<float>(arg) * c_norm_vel;  // VEL normalization by 1 / 16383
        m_type = DecoderEventType::KeyUp;
      }
    }
  }
  return m_type != DecoderEventType::UNKNOWN;
}

DecoderEventType TCDDecoder::getEventType() const
{
  return m_type;
}

int TCDDecoder::getKeyOrController() const
{
  return keyOrController;
}

float TCDDecoder::getValue() const
{
  return value;
}

void TCDDecoder::reset()
{
  value = 0;
  m_type = DecoderEventType::UNKNOWN;
  keyOrController = -1;
}
