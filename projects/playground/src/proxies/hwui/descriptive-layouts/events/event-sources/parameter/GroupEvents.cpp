#include "GroupEvents.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <parameters/Parameter.h>
#include <proxies/hwui/HWUI.h>

DescriptiveLayouts::CurrentParameterGroupLockStatus::CurrentParameterGroupLockStatus()
{
  m_locksConnection = Application::get().getPresetManager()->getEditBuffer()->onLocksChanged(
      sigc::mem_fun(this, &CurrentParameterGroupLockStatus::onLockChanged));
}

DescriptiveLayouts::CurrentParameterGroupLockStatus::~CurrentParameterGroupLockStatus()
{
  m_locksConnection.disconnect();
}

void DescriptiveLayouts::CurrentParameterGroupLockStatus::onSelectedParameterChanged(const Parameter *p)
{
  setValue(p ? p->isLocked() : false);
}

void DescriptiveLayouts::CurrentParameterGroupLockStatus::onLockChanged()
{
  onSelectedParameterChanged(Application::get().getPresetManager()->getEditBuffer()->getSelected(
      Application::get().getHWUI()->getCurrentVoiceGroup()));
}
