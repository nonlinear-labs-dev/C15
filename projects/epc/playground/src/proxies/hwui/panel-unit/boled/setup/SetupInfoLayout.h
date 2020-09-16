#pragma once
#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include "SetupInfoTexts.h"

class SetupInfoLayout : public ControlWithChildren
{
 public:
  explicit SetupInfoLayout(SetupInfoEntries entry);

 private:
  MultiLineLabel *m_content;
};
