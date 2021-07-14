#include "PlayModeRibbonBehaviourLabel.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include <parameters/RibbonParameter.h>

PlayModeRibbonBehaviourLabel::PlayModeRibbonBehaviourLabel(const ParameterId &paramID, const Rect &rect)
    : super(rect)
{
  getEditBuffer()->findParameterByID(paramID)->onParameterChanged(
      mem_fun(this, &PlayModeRibbonBehaviourLabel::updateText));
}

PlayModeRibbonBehaviourLabel::~PlayModeRibbonBehaviourLabel()
{
}

void PlayModeRibbonBehaviourLabel::updateText(const Parameter *s)
{
  if(auto ribbonParameter = dynamic_cast<const RibbonParameter *>(s))
  {
    switch(ribbonParameter->getRibbonTouchBehaviour())
    {
      case RibbonTouchBehaviour::ABSOLUTE:
        setText(StringAndSuffix { "a" });
        break;

      case RibbonTouchBehaviour::RELATIVE:
        setText(StringAndSuffix { "r" });
        break;

      default:
        g_assert_not_reached();
        break;
    }
  }
}

EditBuffer *PlayModeRibbonBehaviourLabel::getEditBuffer() const
{
  return Application::get().getPresetManager()->getEditBuffer();
}
