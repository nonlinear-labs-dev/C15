#include "BaseUnitUIMode.h"
#include "Settings.h"
#include "http/UpdateDocumentMaster.h"
#include "Application.h"
#include "proxies/playcontroller/PlaycontrollerProxy.h"

BaseUnitUIMode::BaseUnitUIMode(Settings &settings)
    : super(settings, BaseUnitUIModes::Play)
{
}

BaseUnitUIMode::~BaseUnitUIMode()
{
}

const std::vector<Glib::ustring> &BaseUnitUIMode::enumToString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "play", "parameter-edit", "banks", "presets" };
  return s_modeNames;
}

const std::vector<Glib::ustring> &BaseUnitUIMode::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "Play", "Parameter Edit", "Banks", "Presets" };
  return s_modeNames;
}

void BaseUnitUIMode::syncExternals(SendReason reason) const
{
  auto m = get();
  if(m != BaseUnitUIModes::ParameterEdit)
    m = BaseUnitUIModes::Play;
  auto v = static_cast<uint16_t>(m);
  Application::get().getPlaycontrollerProxy()->sendSetting(BASE_UNIT_UI_MODE, v);
}
