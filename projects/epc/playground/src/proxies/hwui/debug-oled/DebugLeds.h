#pragma once

#include "proxies/hwui/Layout.h"

class DebugLeds : public Layout
{
 private:
  typedef Layout super;

 public:
  explicit DebugLeds(OLEDProxy& oled);
  ~DebugLeds() override;

  void setLedState(int led, bool onOrOff);

  DebugLeds(const DebugLeds& other) = delete;
  DebugLeds& operator=(const DebugLeds&) = delete;
};
