#include "DebugOLED.h"
#include "Application.h"
#include "DebugLeds.h"
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/FrameBuffer.h>

DebugOLED::DebugOLED()
    : OLEDProxy(Rect(0, 96, 256, 64))
{
  reset(new DebugLeds(*this));
}

DebugOLED::~DebugOLED()
{
}
