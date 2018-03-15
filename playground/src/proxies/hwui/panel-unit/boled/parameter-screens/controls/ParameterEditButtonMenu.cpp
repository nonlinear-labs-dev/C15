#include <Application.h>
#include <parameters/MacroControlParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ParameterEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/EditMCInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/RenameMCLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <http/UndoScope.h>

int ParameterEditButtonMenu::s_lastAction = 0;

ParameterEditButtonMenu::ParameterEditButtonMenu(const Rect &rect) :
    super(rect)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  eb->onLocksChanged(mem_fun(this, &ParameterEditButtonMenu::setup));
}

ParameterEditButtonMenu::~ParameterEditButtonMenu()
{
}

void ParameterEditButtonMenu::setup()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();

  clear();
  clearActions();

  if(eb->getSelected()->getParentGroup()->areAllParametersLocked())
    addButton("Unlock Group", bind(&ParameterEditButtonMenu::toggleGroupLock, this));
  else
    addButton("Lock Group", bind(&ParameterEditButtonMenu::toggleGroupLock, this));

  addButton("Lock all", bind(&ParameterEditButtonMenu::lockAll, this));

  if(eb->hasLocks())
    addButton("Unlock all", bind(&ParameterEditButtonMenu::unlockAll, this));

  eb->onSelectionChanged(sigc::mem_fun(this, &ParameterEditButtonMenu::onParameterSelectionChanged));
  selectButton(s_lastAction);
}

void ParameterEditButtonMenu::onParameterSelectionChanged(Parameter *oldParameter, Parameter *newParameter)
{
  auto newGroup = newParameter->getParentGroup();

  if(m_currentGroup != newGroup)
  {
    m_currentGroup = newGroup;

    m_connection.disconnect();
    m_connection = m_currentGroup->onGroupChanged(sigc::mem_fun(this, &ParameterEditButtonMenu::onGroupChanged));
  }
}

void ParameterEditButtonMenu::onGroupChanged()
{
  auto allParametersLocked = m_currentGroup->areAllParametersLocked();

  if(m_allParametersLocked != allParametersLocked)
  {
    m_allParametersLocked = allParametersLocked;
    setItemTitle(0, m_allParametersLocked ? "Unlock Group" : "Lock Group");
  }
}

void ParameterEditButtonMenu::selectButton(size_t i)
{
  super::selectButton(i);
  s_lastAction = i;
}

void ParameterEditButtonMenu::toggleGroupLock()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto group = eb->getSelected()->getParentGroup();
  if(group->areAllParametersLocked())
  {
    auto scope = Application::get().getUndoScope()->startTransaction("Unlock Group");
    eb->getSelected()->getParentGroup()->undoableUnlock(scope->getTransaction());
  }
  else
  {
    auto scope = Application::get().getUndoScope()->startTransaction("Lock Group");
    eb->getSelected()->getParentGroup()->undoableLock(scope->getTransaction());
  }
}

void ParameterEditButtonMenu::unlockAll()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto scope = Application::get().getUndoScope()->startTransaction("Unlock all");
  eb->undoableUnlockAllGroups(scope->getTransaction());
}

void ParameterEditButtonMenu::lockAll()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto scope = Application::get().getUndoScope()->startTransaction("Lock all");
  eb->undoableLockAllGroups(scope->getTransaction());
}
