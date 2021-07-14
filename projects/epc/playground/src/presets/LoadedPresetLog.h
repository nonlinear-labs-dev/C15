#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <sigc++/trackable.h>
#include <nltools/threading/Throttler.h>
#include <nltools/nlohmann/json.hpp>

namespace nltools
{
  namespace msg
  {
    class WebSocketJsonAPI;
  }
}

class LoadedPresetLog : public sigc::trackable
{
 public:
  LoadedPresetLog();
  ~LoadedPresetLog();

  void freeze();
  void thaw();
  void update(const std::string &currentInfo);

 private:
  void onPresetLoaded();
  void add(const std::string &info);
  nlohmann::json onApiCall(const nlohmann::json &in) const;

  Throttler m_throttler;

  bool m_frozen = false;
  std::string m_frozenValue;

  std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> m_log;
  std::unique_ptr<nltools::msg::WebSocketJsonAPI> m_api;
};
