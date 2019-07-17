#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class LayoutModeView : public SetupLabel
{
 private:
  typedef SetupLabel super;

 public:
  LayoutModeView();
  ~LayoutModeView() override;

 private:
  void onSettingChanged(const Setting *s);
};
