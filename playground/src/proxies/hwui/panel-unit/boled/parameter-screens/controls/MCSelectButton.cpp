#include "MCSelectButton.h"
#include <proxies/hwui/buttons.h>
#include <parameters/ModulateableParameter.h>

MCSelectButton::MCSelectButton(Buttons id)
    : super("MC Sel", id)
{
}

MCSelectButton::~MCSelectButton() = default;

void MCSelectButton::update(const Parameter *parameter)
{
  if(auto p = dynamic_cast<const ModulateableParameter *>(parameter))
  {
    if(p->isModSourceChanged())
    {
      setText("MC Sel*");
      return;
    }
  }
  setText("MC Sel");
}
