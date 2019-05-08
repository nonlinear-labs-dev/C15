#include <device-settings/PedalType.h>
#include <proxies/lpc/LPCProxy.h>
#include <http/UpdateDocumentMaster.h>
#include "Application.h"
#include "proxies/lpc/LPCProxy.h"

PedalType::PedalType(Settings &settings, uint16_t lpcKey)
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

const vector<Glib::ustring> &PedalType::enumToString() const
{
  static vector<Glib::ustring> s_modeNames
      = { "pot-tip-active", "pot-ring-active", "switch-closing", "switch-opening" };
  return s_modeNames;
}

const vector<Glib::ustring> &PedalType::enumToDisplayString() const
{
  static vector<Glib::ustring> s_modeNames
      = { "Pot Tip Active", "Pot Ring Active", "Switch Closing", "Switch Opening" };
  return s_modeNames;
}
