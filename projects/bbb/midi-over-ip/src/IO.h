#pragma once

#include <algorithm>
#include <future>
#include <alsa/asoundlib.h>

namespace nltools
{
  namespace msg
  {
    namespace Midi
    {
      class SimpleMessage;
    }
  }
}

class IO
{
 protected:
  IO(snd_rawmidi_t *handle);
  IO(IO &&i);
  ~IO();

  snd_rawmidi_t *m_handle = nullptr;
};

class Input : public IO
{
 public:
  Input(Input &&i);
  Input(const std::string &name);
  ~Input();

  void fetchChunk();

 private:
  static snd_rawmidi_t *open(const std::string &name);

  void readMidi();
  bool setupDevice(size_t bufferSize);
  size_t fetchChunk(uint8_t *buffer, size_t length, int numPollFDs, pollfd *pollFileDescriptors);
  void processChunk(uint8_t *buffer, size_t length, snd_midi_event_t *encoder, snd_midi_event_t *decoder);
  size_t logReserve(snd_rawmidi_status_t *pStatus, size_t bufferSize, size_t oldReserve) const;

  std::future<void> m_bg;
  bool m_quit = false;
  int m_cancelPipe[2];
};

class Output : public IO
{
 public:
  Output(Output &&i);
  Output(const std::string &name);
  void send(const nltools::msg::Midi::SimpleMessage &msg);

 private:
  static snd_rawmidi_t *open(const std::string &name);
};
