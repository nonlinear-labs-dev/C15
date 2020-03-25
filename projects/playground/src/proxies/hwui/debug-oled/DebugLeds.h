#pragma once

#include "proxies/hwui/Layout.h"

class DebugLeds : public Layout
{
 private:
  typedef Layout super;

 public:
  DebugLeds(OLEDProxy& oled);
  virtual ~DebugLeds();

  void setLedState(int led, bool onOrOff);

 private:
  DebugLeds(const DebugLeds& other) = delete;
  DebugLeds& operator=(const DebugLeds&) = delete;
};
