#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupEditor.h>

class UpdateAvailableEditor : public MultiLineLabel, public SetupEditor
{
  typedef MultiLineLabel super;

 public:
  UpdateAvailableEditor();
  ~UpdateAvailableEditor() override;
};
