#include "AlsaMidiOutput.h"
#include "io/Log.h"

AlsaMidiOutput::AlsaMidiOutput(const std::string &device)
{
  if(snd_rawmidi_open(nullptr, &m_handle, device.c_str(), SND_RAWMIDI_NONBLOCK))
    Log::error("Could not open midi output device");
}

AlsaMidiOutput::~AlsaMidiOutput()
{
  if(auto h = std::exchange(m_handle, nullptr))
    snd_rawmidi_close(h);
}

void AlsaMidiOutput::send(const uint8_t *data, size_t numBytes)
{
  if(m_handle)
  {
    if(auto res = snd_rawmidi_write(m_handle, data, numBytes))
    {
      if(size_t(res) != numBytes)
      {
        Log::error("Could not write message into midi output device");
      }
    }

    snd_rawmidi_drain(m_handle);
  }
}
