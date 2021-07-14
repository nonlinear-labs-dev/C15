#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>
#include <nltools/threading/Expiration.h>

class DeviceInformationItem;

class DeviceInfoItemView : public SetupLabel
{
 private:
  typedef SetupLabel base_type;

 public:
  using Duration = std::chrono::steady_clock::duration;

  explicit DeviceInfoItemView(DeviceInformationItem* item, Duration updateInterval = Duration::zero());
  ~DeviceInfoItemView() override;

 private:
  void onInfoChanged(const DeviceInformationItem* s);

  Expiration m_update;
};
