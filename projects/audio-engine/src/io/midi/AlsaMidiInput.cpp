#include "AlsaMidiInput.h"
#include <nltools/logging/Log.h>
#include "io/HighPriorityTask.h"
#include <nltools/ExceptionTools.h>
#include <giomm.h>

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

  snd_midi_event_new(128, &parser);

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
    int rawIdx = 0;

    while(m_run)
    {
      auto readResult = snd_rawmidi_read(m_handle, &byte, 1);

      if(readResult == 1)
      {
        if(rawIdx < 3)
          e.raw[rawIdx++] = byte;

        snd_seq_event_t event;

        if(snd_midi_event_encode_byte(parser, byte, &event) == 1)
        {
          if(event.type != SND_SEQ_EVENT_NONE)
          {
            getCallback()(e);
            break;
          }

          rawIdx = 0;
        }
      }
      else if(readResult == -19)
      {
        nltools::throwException("Could not read from midi input file descriptor =>", snd_strerror(readResult));
      }
    }
  }
}
