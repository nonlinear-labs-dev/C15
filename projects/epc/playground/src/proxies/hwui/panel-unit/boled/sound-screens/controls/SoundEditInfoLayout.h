#pragma once
#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupInfoTexts.h>

class SoundEditInfoLayout : public ControlWithChildren
{
 public:
  explicit SoundEditInfoLayout(SetupInfoEntries entry);
};