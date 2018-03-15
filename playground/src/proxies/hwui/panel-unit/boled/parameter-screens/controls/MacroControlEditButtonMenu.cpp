#include <Application.h>
#include <parameters/MacroControlParameter.h>
#include <presets/EditBuffer.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/panel-unit/boled/BOLED.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MacroControlEditButtonMenu.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/EditMCInfoLayout.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/RenameMCLayout.h>
#include <proxies/hwui/panel-unit/EditPanel.h>
#include <proxies/hwui/panel-unit/PanelUnit.h>
#include <http/UndoScope.h>

int MacroControlEditButtonMenu::s_lastAction = 0;

MacroControlEditButtonMenu::MacroControlEditButtonMenu(const Rect &rect) :
    super(rect)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  eb->onLocksChanged(mem_fun(this, &MacroControlEditButtonMenu::setup));
}

MacroControlEditButtonMenu::~MacroControlEditButtonMenu()
{
}

void MacroControlEditButtonMenu::setup()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();

  clear();
  clearActions();

  addButton("Smoothing", bind(&MacroControlEditButtonMenu::smoothing, this));
  addButton("Rename", bind(&MacroControlEditButtonMenu::rename, this));
  addButton("Mod Reset", bind(&MacroControlEditButtonMenu::reset, this));

  if(eb->getSelected()->getParentGroup()->areAllParametersLocked())
    addButton("Unlock Group", bind(&MacroControlEditButtonMenu::unlockGroup, this));
  else
    addButton("Lock Group", bind(&MacroControlEditButtonMenu::lockGroup, this));

  addButton("Lock all", bind(&MacroControlEditButtonMenu::lockAll, this));

  if(eb->hasLocks())
    addButton("Unlock all", bind(&MacroControlEditButtonMenu::unlockAll, this));

  auto mcGroup = eb->getParameterGroupByID("MCs");
  mcGroup->onGroupChanged(mem_fun(this, &MacroControlEditButtonMenu::onGroupChanged));

  selectButton(s_lastAction);
}

void MacroControlEditButtonMenu::onGroupChanged()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto mcGroup = eb->getParameterGroupByID("MCs");
  auto allParametersLocked = mcGroup->areAllParametersLocked();
  setItemTitle(3, allParametersLocked ? "Unlock Group" : "Lock Group");
}

void MacroControlEditButtonMenu::selectButton(size_t i)
{
  super::selectButton(i);
  s_lastAction = i;
}

void MacroControlEditButtonMenu::rename()
{
  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().setOverlay(new RenameMCLayout());
}

void MacroControlEditButtonMenu::smoothing()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  const auto currentID = eb->getSelected()->getID();
  const auto diffBetweenMacroControlIDAndItsSmoothing = 81;
  const auto smoothingID = currentID + diffBetweenMacroControlIDAndItsSmoothing;
  eb->undoableSelectParameter(smoothingID);
}

void MacroControlEditButtonMenu::editInfo()
{
  Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().setOverlay(new EditMCInfoLayout());
}

void MacroControlEditButtonMenu::reset()
{
  if(auto p = dynamic_cast<MacroControlParameter*>(Application::get().getPresetManager()->getEditBuffer()->getSelected()))
  {
    p->undoableResetConnectionsToTargets();
  }
}

void MacroControlEditButtonMenu::unlockGroup()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto scope = Application::get().getUndoScope()->startTransaction("Unlock Group");
  eb->getSelected()->getParentGroup()->undoableUnlock(scope->getTransaction());
}

void MacroControlEditButtonMenu::lockGroup()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto scope = Application::get().getUndoScope()->startTransaction("Lock Group");
  eb->getSelected()->getParentGroup()->undoableLock(scope->getTransaction());
}

void MacroControlEditButtonMenu::unlockAll()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto scope = Application::get().getUndoScope()->startTransaction("Unlock all");
  eb->undoableUnlockAllGroups(scope->getTransaction());
}

void MacroControlEditButtonMenu::lockAll()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto scope = Application::get().getUndoScope()->startTransaction("Lock all");
  eb->undoableLockAllGroups(scope->getTransaction());
}
