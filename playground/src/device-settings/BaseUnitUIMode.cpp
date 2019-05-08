#include "BaseUnitUIMode.h"
#include "Settings.h"
#include "http/UpdateDocumentMaster.h"
#include "Application.h"
#include "proxies/lpc/LPCProxy.h"

BaseUnitUIMode::BaseUnitUIMode(Settings &settings)
    : super(settings, BaseUnitUIModes::Play)
{
}

BaseUnitUIMode::~BaseUnitUIMode()
{
}

bool BaseUnitUIMode::set(BaseUnitUIModes m)
{
  if(super::set(m))
  {
    if(m != BaseUnitUIModes::ParameterEdit)
      m = BaseUnitUIModes::Play;

    uint16_t v = (uint16_t) m;
    Application::get().getLPCProxy()->sendSetting(BASE_UNIT_UI_MODE, v);
    return true;
  }
  return false;
}

const vector<Glib::ustring> &BaseUnitUIMode::enumToString() const
{
  static vector<Glib::ustring> s_modeNames = { "play", "parameter-edit", "banks", "presets" };
  return s_modeNames;
}

const vector<Glib::ustring> &BaseUnitUIMode::enumToDisplayString() const
{
  static vector<Glib::ustring> s_modeNames = { "Play", "Parameter Edit", "Banks", "Presets" };
  return s_modeNames;
}
