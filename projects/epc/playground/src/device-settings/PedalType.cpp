#include <device-settings/PedalType.h>
#include <proxies/playcontroller/PlaycontrollerProxy.h>
#include <http/UpdateDocumentMaster.h>
#include <playcontroller/EHC-pedal-presets.h>
#include "Application.h"
#include "proxies/playcontroller/PlaycontrollerProxy.h"
#include <algorithm>

PedalType::PedalType(UpdateDocumentContributor &settings, uint16_t playcontrollerKey)
    : super(settings, PedalTypes::PotTipActive)
    , m_playcontrollerKey(playcontrollerKey)
{
}

void PedalType::incDec(int dir, bool wrap)
{
  auto &v = getDisplayStrings();
  auto currDisplayString = getDisplayString();
  auto currPos = std::find(v.begin(), v.end(), currDisplayString);
  if(currPos == v.end())
    nltools_assertNotReached();

  auto idx = std::distance(v.begin(), currPos);

  idx += dir;

  if(idx < 0 && wrap)
    idx = v.size() + idx;

  if(idx >= v.size() && wrap)
    idx = idx - v.size();

  if(idx >= 0 && idx < v.size())
  {
    if(auto p = EHC_GetPresetByDisplayName(v[idx].c_str()))
      set((tEnum) p->id);
  }
}

Glib::ustring PedalType::getDisplayString() const
{
  auto v = getAllValues<PedalTypes>();
  auto it = std::find(v.begin(), v.end(), get());
  if(it != v.end())
  {
    auto idx = std::distance(v.begin(), it);
    return getDisplayStrings().at(idx);
  }

  return "";
}

const std::vector<Glib::ustring> &PedalType::getDisplayStrings() const
{
  static std::vector<Glib::ustring> s_modeNames;

  if(s_modeNames.empty())
  {
    forEachValue<PedalTypes>([&](PedalTypes e) {
      auto index = static_cast<EHC_PRESET_ID>(e);
      if(auto p = EHC_GetPresetById(index))
        s_modeNames.emplace_back(p->displayName);
    });
  }

  return s_modeNames;
}

PedalType::~PedalType() = default;

void PedalType::syncExternals(SendReason reason) const
{
  auto reset = reason == SendReason::SettingChanged;
  Application::get().getPlaycontrollerProxy()->sendPedalSetting(m_playcontrollerKey, get(), reset);
}

void PedalType::load(const Glib::ustring &text, Initiator initiator)
{
  static std::map<std::string, std::string> m { { "pot-tip-active", "PotTipActive" },
                                                { "pot-ring-active", "PotRingActive" },
                                                { "switch-closing", "SwitchClosing" },
                                                { "switch-opening", "SwitchOpening" } };

  auto it = m.find(text);
  if(it != m.end())
    super::load(it->second, initiator);
  else
    super::load(text, initiator);
}

bool PedalType::set(PedalTypes m)
{
  return forceset(m);
}
