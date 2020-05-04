#include <Application.h>
#include "ScopedLock.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "parameters/Parameter.h"

ScopedLock::ScopedLock(UNDO::Transaction* trans)
    : m_transaction { trans }
{
}

ScopedLock::~ScopedLock()
{
  for(const auto& param : m_oldLockedParameters)
  {
    param->undoableUnlock(m_transaction);
  }
}

void ScopedLock::addLock(const ParameterId& id)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  if(auto param = eb->findParameterByID(id))
  {
    addLock(param);
  }
}

void ScopedLock::addLock(Parameter* param)
{
  if(param && !param->isLocked())
  {
    m_oldLockedParameters.emplace_back(param);
    param->undoableLock(m_transaction);
  }
}

void ScopedLock::addGroupLock(const GroupId& id)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  if(auto group = eb->getParameterGroupByID(id))
  {
    for(auto& p : group->getParameters())
    {
      addLock(p);
    }
  }
}

void ScopedLock::addLocks(const std::vector<Parameter*>& params)
{
  for(auto p : params)
  {
    addLock(p);
  }
}
