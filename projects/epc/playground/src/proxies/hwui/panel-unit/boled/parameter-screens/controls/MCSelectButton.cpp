#include "MCSelectButton.h"
#include <proxies/hwui/buttons.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/ModulateableParameter.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/ParameterLayout.h>

MCSelectButton::MCSelectButton(Buttons id)
    : super("MC Sel", id)
{
}

MCSelectButton::~MCSelectButton() = default;

void MCSelectButton::update(const Parameter *parameter)
{
  if(parameter == nullptr || parameter->isDisabled())
  {
    setText(StringAndSuffix::empty());
    return;
  }

  if(auto p = dynamic_cast<const ModulateableParameter *>(parameter))
  {
    if(p->isModSourceChanged())
    {
      setText(StringAndSuffix { "MC Sel*" });
      return;
    }
  }
  setText(StringAndSuffix { "MC Sel" });
}
