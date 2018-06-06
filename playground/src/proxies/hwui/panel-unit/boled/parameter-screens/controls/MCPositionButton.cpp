#include "MCPositionButton.h"
#include "parameters/ModulateableParameter.h"
#include <proxies/hwui/buttons.h>

MCPositionButton::MCPositionButton (Buttons id) :
    super ("MC Pos", id)
{

}

MCPositionButton::~MCPositionButton ()
{
}


void MCPositionButton::update (const Parameter *parameter)
{
  if (const ModulateableParameter *p = dynamic_cast<const ModulateableParameter*> (parameter))
  {
    if (p->getModulationSource () == ModulateableParameter::NONE)
      setText("");
    else
      setText("MC Pos");
  }
  else
  {
    setText("");
  }
}
