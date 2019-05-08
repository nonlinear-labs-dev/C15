#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class EditSmoothingTimeView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  EditSmoothingTimeView();
  virtual ~EditSmoothingTimeView();

 private:
  void onSettingChanged(const Setting *s);
};
