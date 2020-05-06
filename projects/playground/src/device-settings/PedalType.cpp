#include <device-settings/PedalType.h>
#include <proxies/lpc/LPCProxy.h>
#include <http/UpdateDocumentMaster.h>
#include <EHC-pedal-presets.h>
#include "Application.h"
#include "proxies/lpc/LPCProxy.h"

PedalType::PedalType(UpdateDocumentContributor &settings, uint16_t lpcKey)
    : super(settings, PedalTypes::PotTipActive)
    , m_lpcKey(lpcKey)
{
}

PedalType::~PedalType()
{
}

void PedalType::sendToLPC() const
{
  Application::get().getLPCProxy()->sendSetting(m_lpcKey, (uint16_t)(get()));
}

const std::vector<Glib::ustring> &PedalType::enumToString() const
{
  static std::vector<Glib::ustring> s_modeNames;

  if(s_modeNames.empty())
  {
    for(auto n : getAllStrings<PedalTypes>())
    {
      s_modeNames.emplace_back(n);
    }
  }

  return s_modeNames;
}

const std::vector<Glib::ustring> &PedalType::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames;

  if(s_modeNames.empty())
  {
    forEachValue<PedalTypes>([&](PedalTypes e) {
      auto index = static_cast<int>(e);
      s_modeNames.emplace_back(EHC_presets[index].name);
    });
  }

  return s_modeNames;
}
