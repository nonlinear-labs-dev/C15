#pragma once

#include "proxies/hwui/controls/ControlWithChildren.h"

class DebugLedPanel : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  DebugLedPanel(const Rect& pos, int startID);
  ~DebugLedPanel() override;

  bool setLedState(int led, bool onOrOff);
  DebugLedPanel(const DebugLedPanel& other) = delete;
  DebugLedPanel& operator=(const DebugLedPanel&) = delete;
};
