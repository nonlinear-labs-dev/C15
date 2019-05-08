#include "ParamEditModeRibbonBehaviourLabel.h"
#include "Application.h"
#include "device-settings/Settings.h"
#include "device-settings/ParameterEditModeRibbonBehaviour.h"

ParamEditModeRibbonBehaviourLabel::ParamEditModeRibbonBehaviourLabel(const Rect& rect)
    : super(rect)
{
  getSetting()->onChange(mem_fun(this, &ParamEditModeRibbonBehaviourLabel::updateText));
}

ParamEditModeRibbonBehaviourLabel::~ParamEditModeRibbonBehaviourLabel()
{
}

void ParamEditModeRibbonBehaviourLabel::updateText(const Setting* s)
{
  switch(dynamic_cast<const ParameterEditModeRibbonBehaviour*>(s)->get())
  {
    case ParameterEditModeRibbonBehaviours::PARAMETER_EDIT_MODE_RIBBON_BEHAVIOUR_RELATIVE:
      setText("r");
      break;

    case ParameterEditModeRibbonBehaviours::PARAMETER_EDIT_MODE_RIBBON_BEHAVIOUR_ABSOLUTE:
      setText("a");
      break;
  }
}

shared_ptr<ParameterEditModeRibbonBehaviour> ParamEditModeRibbonBehaviourLabel::getSetting() const
{
  return getSettings()->getSetting<ParameterEditModeRibbonBehaviour>();
}

shared_ptr<Settings> ParamEditModeRibbonBehaviourLabel::getSettings() const
{
  return Application::get().getSettings();
}
