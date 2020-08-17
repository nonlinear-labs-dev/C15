#include "AlsaMidiInput.h"
#include <nltools/logging/Log.h>
#include "io/HighPriorityTask.h"
#include <nltools/ExceptionTools.h>
#include <giomm.h>

#include "stdio.h"

AlsaMidiInput::AlsaMidiInput(const std::string &deviceName, Callback cb)
    : MidiInput(cb)
    , m_cancellable(Gio::Cancellable::create())
{
  open(deviceName);
}

AlsaMidiInput::~AlsaMidiInput()
{
  close();
}

void AlsaMidiInput::open(const std::string &deviceName)
{
  if(snd_rawmidi_open(&m_handle, nullptr, deviceName.c_str(), SND_RAWMIDI_NONBLOCK))
    nltools::Log::error("Could not open midi device");
}

void AlsaMidiInput::start()
{
  m_run = true;
  m_task = std::make_unique<HighPriorityTask>(2, [=]() { doBackgroundWork(); });
}

void AlsaMidiInput::stop()
{
  m_run = false;
  m_cancellable->cancel();
  m_task.reset();
}

void AlsaMidiInput::close()
{
  if(auto h = std::exchange(m_handle, nullptr))
    snd_rawmidi_close(h);
}

void AlsaMidiInput::doBackgroundWork()
{
  uint8_t byte;

  snd_midi_event_t *parser = nullptr;
  snd_midi_event_new(512, &parser);  // a chunk of raw SysEx will at most be this size

  int numPollFDs = snd_rawmidi_poll_descriptors_count(m_handle);

  pollfd pollFileDescriptors[numPollFDs + 1];
  numPollFDs = snd_rawmidi_poll_descriptors(m_handle, pollFileDescriptors, numPollFDs);
  pollFileDescriptors[numPollFDs].fd = m_cancellable->get_fd();
  pollFileDescriptors[numPollFDs].events = POLLIN;

  while(true)
  {
    switch(poll(pollFileDescriptors, numPollFDs + 1, -1))
    {
      case POLLPRI:
      case POLLRDHUP:
      case POLLERR:
      case POLLHUP:
      case POLLNVAL:
        throw std::runtime_error("Polling the midi input file descriptor failed. Terminating.");

      default:
        break;
    }

    if(!m_run)
      break;

    MidiEvent e;
    snd_seq_event_t event;
    int rawIdx = 0;

    while(m_run)
    {
      auto readResult = snd_rawmidi_read(m_handle, &byte, 1);

      if(readResult == -19)
        nltools::throwException("Could not read from midi input file descriptor =>", snd_strerror(readResult));
      if(readResult != 1)  // one byte successfully read ?
        continue;          // ... no

      if(rawIdx < 3)             // we record raw bytes here because pitchbend event data "event.data.control.*" ...
        e.raw[rawIdx++] = byte;  // ... does not nicely cover the 14 bit value range

      if(snd_midi_event_encode_byte(parser, byte, &event) == 1)
      {  // have a finished MIDI message
        switch(event.type)
        {
          case SND_SEQ_EVENT_PITCHBEND:  // TCD data (wrapped in PitchBends)
            printf("\nTCD Data : %02X %02X %02X\n", e.raw[0], e.raw[1], e.raw[2]);
            fflush(stdout);
            getCallback()(e);
            break;
          case SND_SEQ_EVENT_SYSEX:  // chunck of SysEx data (may not be a complete packet, scan for terminating "0xF7")
            uint8_t *p = (uint8_t *) event.data.ext.ptr;
            printf("\nSysEx Data (%i bytes) : ", event.data.ext.len);
            for(int i = 0; i < event.data.ext.len; i++)
              printf("%02X ", p[i]);
            printf("\n");
            fflush(stdout);
            break;
        }
        rawIdx = 0;
        break;  // break out of "while(m_run){}" after ANY event, seems to be required to keep thread from stalling ???              }
      }
    }  // while(m_run)
  }    // while(true)
}
