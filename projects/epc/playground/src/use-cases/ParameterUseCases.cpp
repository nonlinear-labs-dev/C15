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

void ParameterUseCases::toggleLoadDefault()
{
  auto scope = m_parameter->getUndoScope().startTransaction("Set '%0'", m_parameter->getGroupAndParameterName());
  m_parameter->toggleLoadDefaultValue(scope->getTransaction());
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
  if(!m_parameter->isDisabled())
  {
    auto scope = m_parameter->getUndoScope().startContinuousTransaction(m_parameter, "Set '%0'",
                                                                        m_parameter->getGroupAndParameterName());
    m_parameter->stepCP(scope->getTransaction(), incs, fine, shift);
  }
}

void ParameterUseCases::setDefault()
{
  auto name = m_parameter->getGroupAndParameterName();
  auto scope = m_parameter->getUndoScope().startTransaction("Set Default '%0'", name);
  m_parameter->setCPFromHwui(scope->getTransaction(), m_parameter->getDefaultValue());
}

void ParameterUseCases::setControlPosition(tControlPositionValue cp)
{
  auto name = m_parameter->getGroupAndParameterName();
  auto scope = m_parameter->getUndoScope().startContinuousTransaction(m_parameter, "Set '%0'", name);
  m_parameter->setCPFromHwui(scope->getTransaction(), cp);
}
