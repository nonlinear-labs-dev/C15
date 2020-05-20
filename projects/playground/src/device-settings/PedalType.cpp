#include <device-settings/PedalType.h>
#include <proxies/lpc/LPCProxy.h>
#include <http/UpdateDocumentMaster.h>
#include <EHC-pedal-presets.h>
#include "Application.h"
#include "proxies/lpc/LPCProxy.h"
#include "xml/Writer.h"
#include "xml/Attribute.h"

PedalType::PedalType(UpdateDocumentContributor &settings, uint16_t lpcKey)
    : super(settings, PedalTypes::PotTipActive)
    , m_lpcKey(lpcKey)
{
}

PedalType::~PedalType() = default;

void PedalType::sendToLPC() const
{
  Application::get().getLPCProxy()->sendPedalSetting(m_lpcKey, (uint16_t)(get()));
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

void PedalType::writeDocument(Writer &writer, UpdateDocumentContributor::tUpdateID knownRevision) const
{
  bool changed = knownRevision < getUpdateIDOfLastChange();
  writer.writeTextElement("value", std::to_string(get()), Attribute("changed", changed));
}

void PedalType::load(const Glib::ustring &text)
{
  static std::map<std::string, std::string> m { { "pot-tip-active", "PotTipActive" },
                                                { "pot-ring-active", "PotRingActive" },
                                                { "switch-closing", "SwitchClosing" },
                                                { "switch-opening", "SwitchOpening" } };

  auto it = m.find(text);
  if(it != m.end())
    super::load(it->second);
  else
    super::load(text);
}
