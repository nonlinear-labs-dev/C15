#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class UpdateAvailableView : public SetupLabel
{
  typedef SetupLabel super;

 public:
  UpdateAvailableView();
  ~UpdateAvailableView() override;

  static bool updateExists();
  static void setUpdateAvailable(bool updateAvailable);

 private:
  static bool m_updateAvailable;
  bool poll();
};
