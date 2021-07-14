#include "HardwareControlEnables.h"
#include <tools/StringTools.h>

HardwareControlEnables::HardwareControlEnables(Settings& s)
    : Setting(s)
{
  for(auto& row: m_data) {
    row.fill(true);
  }
}

bool HardwareControlEnables::getState(HardwareControlEnables::tHWIdx hwIdx,
                                      HardwareControlEnables::tSettingIdx settingIdx) const
{
  return m_data[static_cast<size_t>(hwIdx)][static_cast<size_t>(settingIdx)];
}

void HardwareControlEnables::load(const Glib::ustring& text, Initiator initiator)
{
  if(!text.empty())
  {
    auto hw = StringTools::splitStringOnAnyDelimiter(text, '\n');
    if(hw.size() == 8)
    {
      for(auto hwIdx = 0; hwIdx < static_cast<size_t>(tHWIdx::LENGTH); hwIdx++)
      {
        auto map = StringTools::splitStringOnAnyDelimiter(hw[hwIdx], ',');
        for(auto settingIdx = 0; settingIdx < static_cast<size_t>(tSettingIdx::LENGTH); settingIdx++)
        {
          try
          {
            m_data[hwIdx][settingIdx] = map.at(settingIdx) == "1";
          }
          catch(...)
          {
            m_data[hwIdx][settingIdx] = false;
          }
        }
      }
    }
  }
}

Glib::ustring HardwareControlEnables::save() const
{
  Glib::ustring ret;
  for(auto hw : m_data)
  {
    for(auto map : hw)
    {
      ret += map ? "1," : "0,";
    }
    ret += "\n";
  }
  return ret;
}

Glib::ustring HardwareControlEnables::getDisplayString() const
{
  return { "Not Used" };
}

void HardwareControlEnables::setState(HardwareControlEnables::tHWIdx hwIdx,
                                      HardwareControlEnables::tSettingIdx settingIdx, bool state)
{
  auto& val = m_data.at(static_cast<size_t>(hwIdx)).at(static_cast<size_t>(settingIdx));
  if(val != state)
  {
    val = state;
    notify();
  }
}

const HardwareControlEnables::tData& HardwareControlEnables::getRaw() const
{
  return m_data;
}
