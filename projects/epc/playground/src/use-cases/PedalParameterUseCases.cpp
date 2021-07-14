#include "PedalParameterUseCases.h"
#include <nltools/Assert.h>
#include <parameters/PedalParameter.h>
#include <libundo/undo/Scope.h>

PedalParameterUseCases::PedalParameterUseCases(PedalParameter* p)
    : PhysicalControlParameterUseCases{p}
    , m_param { p }
{
  nltools_assertAlways(m_param != nullptr);
}

void PedalParameterUseCases::setPedalMode(const Glib::ustring& mode)
{
  auto scope = m_param->getUndoScope().startTransaction("Set pedal mode");
  m_param->undoableSetPedalMode(scope->getTransaction(), mode);
}
