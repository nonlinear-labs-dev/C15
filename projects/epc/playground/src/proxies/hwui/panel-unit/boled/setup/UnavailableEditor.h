#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class UnavailableEditor : public SetupLabel, public SetupEditor
{
 public:
  UnavailableEditor();
  ~UnavailableEditor() override;
};
