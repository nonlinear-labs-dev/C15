#include <nltools/Assert.h>
#include "ParameterUseCases.h"
#include "presets/EditBuffer.h"
#include <libundo/undo/Scope.h>

ParameterUseCases::ParameterUseCases(Parameter* parameter)
    : m_parameter { parameter }
{
  nltools_assertAlways(m_parameter != nullptr);
}

void ParameterUseCases::recallParameterFromPreset()
{
  if(m_parameter->isChangedFromLoaded())
  {
    auto& scope = m_parameter->getUndoScope();
    auto name = m_parameter->getGroupAndParameterName();
    auto transactionScope = scope.startTransaction("Recall value from Preset for '%0'", name);
    auto transaction = transactionScope->getTransaction();
    m_parameter->undoableRecallFromPreset(transaction);
  }
}

void ParameterUseCases::undoRecallParameterFromPreset(tControlPositionValue cp)
{
  auto& scope = m_parameter->getUndoScope();
  auto transactionScope = scope.startTransaction("Recall %0 value from Editbuffer", m_parameter->getLongName());
  auto transaction = transactionScope->getTransaction();
  m_parameter->setCPFromHwui(transaction, cp);
}

void ParameterUseCases::incDec(int incs, bool fine, bool shift)
{
  auto scope = m_parameter->getUndoScope().startContinuousTransaction(m_parameter, "Set '%0'",
                                                                      m_parameter->getGroupAndParameterName());
  m_parameter->stepCP(scope->getTransaction(), incs, fine, shift);
}
