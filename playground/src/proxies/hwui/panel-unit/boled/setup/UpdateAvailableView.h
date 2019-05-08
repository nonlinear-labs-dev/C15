#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class UpdateAvailableView : public SetupLabel
{
  typedef SetupLabel super;

 public:
  UpdateAvailableView();
  virtual ~UpdateAvailableView();

  static bool updateExists();

 private:
  bool poll();
};
