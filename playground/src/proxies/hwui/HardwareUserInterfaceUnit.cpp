#include "HardwareUserInterfaceUnit.h"
#include "UsageMode.h"

HardwareUserInterfaceUnit::HardwareUserInterfaceUnit ()
{
}

HardwareUserInterfaceUnit::~HardwareUserInterfaceUnit ()
{
}

void HardwareUserInterfaceUnit::setupFocusAndMode (FocusAndMode focusAndMode)
{
}

void HardwareUserInterfaceUnit::setUsageMode (UsageMode *mode)
{
  if(mode && m_usageMode)
  {
    if(typeid(*mode) == typeid(*m_usageMode.get()))
    {
      delete mode;
      return;
    }
  }

  m_usageMode.reset (mode);
  m_usageMode->setup ();
}

void HardwareUserInterfaceUnit::restoreUsageMode (shared_ptr<UsageMode> mode)
{
  m_usageMode = mode;
}

shared_ptr<UsageMode> HardwareUserInterfaceUnit::getUsageMode ()
{
  return m_usageMode;
}

shared_ptr<const UsageMode> HardwareUserInterfaceUnit::getUsageMode () const
{
  return m_usageMode;
}

bool HardwareUserInterfaceUnit::onButtonPressed(Buttons buttonID, ButtonModifiers modifiers, bool state)
{
  if(m_usageMode)
    return m_usageMode->onButtonPressed (buttonID, modifiers, state);

  return false;
}
