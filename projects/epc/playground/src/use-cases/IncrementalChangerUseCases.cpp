#include <nltools/Assert.h>
#include "IncrementalChangerUseCases.h"
#include <parameters/Parameter.h>
#include <libundo/undo/Scope.h>

IncrementalChangerUseCases::IncrementalChangerUseCases(QuantizedValue::IncrementalChanger* changer)
    : m_changer { changer }
{
  nltools_assertAlways(m_changer != nullptr);
}

void IncrementalChangerUseCases::changeBy(tControlPositionValue amount, bool fine)
{
  if(auto parameter = m_changer->getOwner())
  {
    auto name = parameter->getGroupAndParameterName();
    auto scope = parameter->getUndoScope().startContinuousTransaction(parameter, "Set '%0'", name);
    m_changer->changeBy(scope->getTransaction(), amount, fine);
  }
}
