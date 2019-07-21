#pragma once

#include "MidiInput.h"
#include <giomm.h>
#include <memory>

class HighPriorityTask;

class AlsaMidiInput : public MidiInput
{
 public:
  using Callback = std::function<void(const MidiEvent &)>;

  AlsaMidiInput(const std::string &deviceName, Callback cb);
  ~AlsaMidiInput() override;

  void start() override;
  void stop() override;

 private:
  void open(const std::string &deviceName);
  void close();
  void doBackgroundWork();

  Glib::RefPtr<Gio::Cancellable> m_cancellable;
  snd_rawmidi_t *m_handle = nullptr;
  bool m_run = true;
  std::unique_ptr<HighPriorityTask> m_task;
};
