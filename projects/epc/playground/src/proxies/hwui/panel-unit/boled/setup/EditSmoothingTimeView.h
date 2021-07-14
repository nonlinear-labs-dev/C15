#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class Setting;

class EditSmoothingTimeView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  EditSmoothingTimeView();
  ~EditSmoothingTimeView() override;

 private:
  void onSettingChanged(const Setting *s);
};
