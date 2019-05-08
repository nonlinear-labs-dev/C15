#include "MCAmountButton.h"
#include "parameters/ModulateableParameter.h"
#include <proxies/hwui/buttons.h>

MCAmountButton::MCAmountButton(int id)
    : super("MC Amt", id)
{
}

MCAmountButton::~MCAmountButton() = default;

void MCAmountButton::update(const Parameter *parameter)
{
  if(const auto *p = dynamic_cast<const ModulateableParameter *>(parameter))
  {
    if(p->getModulationSource() == ModulationSource::NONE)
      setText("");
    else
      setText({ std::string("MC Amt") + (p->isModAmountChanged() ? "*" : "") });
  }
  else
  {
    setText("");
  }
}
