#pragma once

#include "proxies/hwui/OLEDProxy.h"

class DebugOLED : public OLEDProxy
{
 private:
  typedef OLEDProxy super;

 public:
  DebugOLED();
  ~DebugOLED() override;
  DebugOLED(const DebugOLED& other) = delete;
  DebugOLED& operator=(const DebugOLED&) = delete;
};
