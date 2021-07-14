#include <nltools/Assert.h>
#include "MacroControlParameterUseCases.h"
#include "presets/EditBuffer.h"
#include "parameters/MacroControlParameter.h"
#include "parameters/ModulateableParameter.h"
#include <libundo/undo/Scope.h>

MacroControlParameterUseCases::MacroControlParameterUseCases(MacroControlParameter* parameter)
    : ParameterUseCases{parameter}
    , m_mc { parameter }
{
  nltools_assertAlways(m_mc != nullptr);
}

void MacroControlParameterUseCases::setName(const Glib::ustring& name)
{
  if(m_mc->getGivenName() != name)
  {
    auto scope = m_mc->getUndoScope().startTransaction("Rename MacroControl '%0'", m_mc->getLongName());
    m_mc->undoableSetGivenName(scope->getTransaction(), name);
  }
}

void MacroControlParameterUseCases::setInfo(const Glib::ustring& info)
{
  if(m_mc->getInfo() != info)
  {
    auto scope = m_mc->getUndoScope().startTransaction("Set MacroControl Info '%0'", m_mc->getLongName());
    m_mc->undoableSetInfo(scope->getTransaction(), info);
  }
}

void MacroControlParameterUseCases::resetConnectionsToTargets()
{
  auto scope = m_mc->getUndoScope().startTransaction("Reset MacroControls connections");

  while(!m_mc->m_targets.empty())
  {
    auto p = *m_mc->m_targets.begin();
    p->undoableSelectModSource(scope->getTransaction(), MacroControls::NONE);
    p->undoableSetModAmount(scope->getTransaction(), 0);
  }
}

void MacroControlParameterUseCases::incDecPosition(int incs, bool fine, bool shift)
{
  auto name = incs > 0 ? "Inc." : "Dec.";
  auto scope = m_mc->getUndoScope().startTransaction(std::to_string(name) + " MC Pos");
  auto transaction = scope->getTransaction();
  m_mc->setCPFromHwui(transaction, m_mc->getNextStepValue(incs, fine, shift));
}