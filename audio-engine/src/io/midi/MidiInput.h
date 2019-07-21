#pragma once

#include "Types.h"
#include <functional>

class MidiInput
{
 public:
  using Callback = std::function<void(const MidiEvent &)>;

  MidiInput(Callback cb);
  virtual ~MidiInput();

  virtual void start();
  virtual void stop();

 protected:
  inline Callback &getCallback()
  {
    return m_cb;
  }

 private:
  Callback m_cb;
};
