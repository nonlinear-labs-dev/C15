#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>
#include <tools/Expiration.h>
#include <chrono>

class DeviceInformationItem;

class DeviceInfoItemView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  using Duration = std::chrono::system_clock::duration;

  DeviceInfoItemView(DeviceInformationItem* item, Duration updateInterval = Duration::zero());
  virtual ~DeviceInfoItemView();

 private:
  void onInfoChanged(const DeviceInformationItem* s);

  Expiration m_update;
};
