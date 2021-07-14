#include "LoadedPresetLog.h"
#include "EditBuffer.h"
#include <tools/Uuid.h>
#include <nltools/messaging/WebSocketJsonAPI.h>

namespace std
{
  static void to_json(nlohmann::json &j, const std::pair<std::chrono::system_clock::time_point, std::string> &in)
  {
    j = { { "time", in.first.time_since_epoch().count() }, { "info", in.second } };
  }
}

LoadedPresetLog::LoadedPresetLog()
    : m_throttler(std::chrono::milliseconds(500))
    , m_api(std::make_unique<nltools::msg::WebSocketJsonAPI>(PLAYGROUND_PRESETLOAD_LOG_PORT,
                                                             [this](const auto &in) { return onApiCall(in); }))
{
}

void LoadedPresetLog::freeze()
{
  m_frozen = true;
  m_frozenValue.clear();
}

void LoadedPresetLog::thaw()
{
  m_frozen = false;

  if(!m_frozenValue.empty())
    add(std::exchange(m_frozenValue, ""));
}

LoadedPresetLog::~LoadedPresetLog() = default;

void LoadedPresetLog::update(const std::string &currentInfo)
{
  if(m_frozen)
    m_frozenValue = currentInfo;
  else
    m_throttler.doTask([=] { add(currentInfo); });
}

void LoadedPresetLog::add(const std::string &info)
{
  if(m_log.empty() || m_log.back().second != info)
  {
    if(!info.empty())
    {
      m_log.emplace_back(std::chrono::system_clock::now(), info);
      m_api->sendAll({ "incrementalUpdate", m_log.back() });
    }
  }
}

nlohmann::json LoadedPresetLog::onApiCall(const nlohmann::json &in) const
{
  if(in.is_object())
  {
    auto obj = in.get<nlohmann::json::object_t>();
    auto it = obj.begin();
    auto name = it->first;
    auto args = it->second;

    if(name == "getAll")
      return { "all", m_log };
  }
  return {};
}
