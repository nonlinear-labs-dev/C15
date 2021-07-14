#pragma once

#include <string>
#include <functional>
#include <vector>
#include <sigc++/trackable.h>

class AlsaDeviceMonitor : public sigc::trackable
{
 public:
  using DeviceNames = std::vector<std::string>;
  using InputDevices = DeviceNames;
  using OutputDevices = DeviceNames;
  using Callback = std::function<void(const InputDevices&, const OutputDevices&)>;

  AlsaDeviceMonitor(Callback cb);
  void setEnabled(bool e);

 private:
  bool poll();

  bool m_enabled = true;

  Callback m_cb;
  InputDevices m_ins;
  OutputDevices m_outs;
};
