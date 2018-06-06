#include "MCAmountButton.h"
#include "parameters/ModulateableParameter.h"
#include <proxies/hwui/buttons.h>

MCAmountButton::MCAmountButton (Buttons id) :
    super ("MC Amt", id)
{
}

MCAmountButton::~MCAmountButton ()
{
}

void MCAmountButton::update (const Parameter *parameter)
{
  if (const ModulateableParameter *p = dynamic_cast<const ModulateableParameter*> (parameter))
  {
    if (p->getModulationSource () == ModulateableParameter::NONE)
      setText("");
    else
      setText("MC Amt");
  }
  else
  {
    setText("");
  }
}

