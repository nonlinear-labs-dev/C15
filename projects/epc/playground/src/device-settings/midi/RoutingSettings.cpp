#include "RoutingSettings.h"
#include <tools/StringTools.h>

RoutingSettings::RoutingSettings(Settings& s)
    : Setting(s)
    , m_data{}
{
  for(auto& row : m_data)
  {
    row.fill(true);
  }
}

bool RoutingSettings::getState(RoutingSettings::tRoutingIndex hwIdx, RoutingSettings::tAspectIndex settingIdx) const
{
  return m_data[static_cast<size_t>(hwIdx)][static_cast<size_t>(settingIdx)];
}

void RoutingSettings::load(const Glib::ustring& text, Initiator initiator)
{
  if(!text.empty())
  {
    auto entries = StringTools::splitStringOnAnyDelimiter(text, '\n');
    auto idx = 0;
    for(const auto& entry : entries)
    {
      auto map = StringTools::splitStringOnAnyDelimiter(entry, ',');
      for(auto settingIdx = 0; settingIdx < static_cast<size_t>(tAspectIndex::LENGTH); settingIdx++)
      {
        try
        {
          m_data.at(idx).at(settingIdx) = map.at(settingIdx) == "1";
        }
        catch(...)
        {
            nltools::Log::error(__PRETTY_FUNCTION__, "Encountered exception while trying to update Routing for indices: (entry)(aspect)", idx, settingIdx);
        }
      }

      idx++;
    }
  }
}

Glib::ustring RoutingSettings::save() const
{
  Glib::ustring ret;
  for(auto routing : m_data)
  {
    for(auto entry : routing)
    {
      ret += entry ? "1," : "0,";
    }
    ret += "\n";
  }
  return ret;
}

Glib::ustring RoutingSettings::getDisplayString() const
{
  return { "Not Used" };
}

void RoutingSettings::setState(RoutingSettings::tRoutingIndex hwIdx, RoutingSettings::tAspectIndex settingIdx, bool state)
{
  auto& val = m_data.at(static_cast<size_t>(hwIdx)).at(static_cast<size_t>(settingIdx));
  if(val != state)
  {
    val = state;
    notify();
  }
}

const RoutingSettings::tData& RoutingSettings::getRaw() const
{
  return m_data;
}

void RoutingSettings::setAllValues(bool value)
{
  bool anyChanged = false;
  for(auto& entry: m_data) {
    for(auto& aspect: entry) {
      anyChanged |= (aspect != value);
      aspect = value;
    }
  }

  if(anyChanged)
    notify();
}
