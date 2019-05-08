#pragma once

#include "proxies/hwui/DFBLayout.h"

class DebugLeds : public DFBLayout
{
 private:
  typedef DFBLayout super;

 public:
  DebugLeds(OLEDProxy& oled);
  virtual ~DebugLeds();

  void setLedState(int led, bool onOrOff);

 private:
  DebugLeds(const DebugLeds& other) = delete;
  DebugLeds& operator=(const DebugLeds&) = delete;
};
