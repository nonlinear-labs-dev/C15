#pragma once

#include <proxies/hwui/controls/DotSlider.h>

class StaticKnubbelSlider : public DotSlider
{
 public:
  StaticKnubbelSlider(tControlPositionValue value, bool biPolar, Rect pos);
};
