#pragma once

#include "proxies/hwui/controls/ControlWithChildren.h"

class DebugLedPanel : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  DebugLedPanel(const Rect& pos, int startID);
  virtual ~DebugLedPanel();

  bool setLedState(int led, bool onOrOff);

 private:
  DebugLedPanel(const DebugLedPanel& other) = delete;
  DebugLedPanel& operator=(const DebugLedPanel&) = delete;
};
