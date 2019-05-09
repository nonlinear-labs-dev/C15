#include "MCPositionButton.h"
#include "parameters/ModulateableParameter.h"
#include <proxies/hwui/buttons.h>

MCPositionButton::MCPositionButton(Buttons id)
    : super("MC Pos", id)
{
}

MCPositionButton::~MCPositionButton() = default;

void MCPositionButton::update(const Parameter *parameter)
{
  if(const auto *p = dynamic_cast<const ModulateableParameter *>(parameter))
  {
    if(p->getModulationSource() == ModulationSource::NONE)
      setText("");
    else
      setText({ std::string("MC Pos") + (p->isMacroControlAssignedAndChanged() ? "*" : "") });
  }
  else
  {
    setText("");
  }
}
