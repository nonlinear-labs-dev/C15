#pragma once

#include "proxies/hwui/controls/Control.h"

class DebugLed : public Control
{
 private:
  typedef Control super;

 public:
  DebugLed(const Rect& pos, int led);
  virtual ~DebugLed();

  void setLedState(bool onOrOff);
  bool redraw(FrameBuffer& fb) override;
  int getId() const;

 private:
  DebugLed(const DebugLed& other) = delete;
  DebugLed& operator=(const DebugLed&) = delete;

  int m_ledId;
  bool m_onOrOff;
};
