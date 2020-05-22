#include <Application.h>
#include <device-settings/ParameterEditModeRibbonBehaviour.h>
#include <device-settings/RibbonRelativeFactor.h>
#include <device-settings/Settings.h>
#include <proxies/lpc/LPCProxy.h>

ParameterEditModeRibbonBehaviour::ParameterEditModeRibbonBehaviour(Settings &settings)
    : super(settings, ParameterEditModeRibbonBehaviours::PARAMETER_EDIT_MODE_RIBBON_BEHAVIOUR_RELATIVE)
{
}

ParameterEditModeRibbonBehaviour::~ParameterEditModeRibbonBehaviour()
{
}

bool ParameterEditModeRibbonBehaviour::set(ParameterEditModeRibbonBehaviours m)
{
  if(super::set(m))
  {
    sendRelatedSettingToLPC();
    return true;
  }
  return false;
}

void ParameterEditModeRibbonBehaviour::sendRelatedSettingToLPC() const
{
  Application::get().getSettings()->getSetting<RibbonRelativeFactor>()->sendToLPC(SendReason::SettingChanged);
}

void ParameterEditModeRibbonBehaviour::sendToLPC(SendReason reason) const
{
  uint16_t v = (uint16_t) get();
  Application::get().getLPCProxy()->sendSetting(PARAMETER_EDIT_MODE_RIBBON_BEHAVIOUR, v);
}

const std::vector<Glib::ustring> &ParameterEditModeRibbonBehaviour::enumToString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "relative", "absolute" };
  return s_modeNames;
}

const std::vector<Glib::ustring> &ParameterEditModeRibbonBehaviour::enumToDisplayString() const
{
  static std::vector<Glib::ustring> s_modeNames = { "Relative", "Absolute" };
  return s_modeNames;
}
