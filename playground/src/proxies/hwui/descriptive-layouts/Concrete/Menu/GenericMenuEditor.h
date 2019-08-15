#pragma once
#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/panel-unit/ButtonReceiver.h>

class GenericMenuEditor : public ControlWithChildren, public ButtonReceiver
{
 public:
  GenericMenuEditor();
};
