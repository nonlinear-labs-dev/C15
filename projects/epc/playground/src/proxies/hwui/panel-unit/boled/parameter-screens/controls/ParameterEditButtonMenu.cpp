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
#include <groups/ParameterGroup.h>

int ParameterEditButtonMenu::s_lastAction = 0;

ParameterEditButtonMenu::ParameterEditButtonMenu(const Rect &rect)
    : super(rect)
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  eb->onLocksChanged(mem_fun(this, &ParameterEditButtonMenu::setup));
}

ParameterEditButtonMenu::~ParameterEditButtonMenu() = default;

void ParameterEditButtonMenu::setup()
{

  clear();

  addActions();

  sanitizeIndex();
  sanitizeLastAction();

  selectButton(s_lastAction);
}

void ParameterEditButtonMenu::addActions()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto vg = Application::get().getHWUI()->getCurrentVoiceGroup();

  if(eb->getSelected(vg)->lockingEnabled())
  {
    if(eb->getSelected(vg)->getParentGroup()->areAllParametersLocked())
      addButton("Unlock Group", std::bind(&ParameterEditButtonMenu::toggleGroupLock, this));
    else
      addButton("Lock Group", std::bind(&ParameterEditButtonMenu::toggleGroupLock, this));
  }

  if(!eb->getSelected(vg)->getParentGroup()->areAllParametersLocked())
    addButton("Lock all", std::bind(&ParameterEditButtonMenu::lockAll, this));

  if(eb->hasLocks(vg))
    addButton("Unlock all", std::bind(&ParameterEditButtonMenu::unlockAll, this));

  eb->onSelectionChanged(sigc::mem_fun(this, &ParameterEditButtonMenu::onParameterSelectionChanged), vg);
}

void ParameterEditButtonMenu::onParameterSelectionChanged(Parameter* oldParameter, Parameter* newParameter,
                                                          SignalOrigin signalType)
{
  auto newGroup = newParameter->getParentGroup();

  if(m_currentGroup != newGroup)
  {
    m_currentGroup = newGroup;

    m_connection.disconnect();
    m_connection = m_currentGroup->onGroupChanged(sigc::mem_fun(this, &ParameterEditButtonMenu::onGroupChanged));
    setup();
  }
}

void ParameterEditButtonMenu::onGroupChanged()
{
  auto allParametersLocked = m_currentGroup->areAllParametersLocked();

  if(m_allParametersLocked != allParametersLocked)
  {
    m_allParametersLocked = allParametersLocked;
    setup();
  }
}

void ParameterEditButtonMenu::sanitizeLastAction()
{
  s_lastAction = sanitizeIndex(s_lastAction);
}

void ParameterEditButtonMenu::selectButton(size_t i)
{
  super::selectButton(i);
  s_lastAction = i;
}

void ParameterEditButtonMenu::toggleGroupLock()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  auto vg = getHWUI()->getCurrentVoiceGroup();
  auto group = eb->getSelected(vg)->getParentGroup();
  EditBufferUseCases useCases(eb);

  if(group->areAllParametersLocked())
    useCases.unlockGroup(group);
  else
    useCases.lockGroup(group);

  setup();
}

void ParameterEditButtonMenu::unlockAll()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  EditBufferUseCases useCase(eb);
  useCase.unlockAllGroups();
  setup();
}

void ParameterEditButtonMenu::lockAll()
{
  auto eb = Application::get().getPresetManager()->getEditBuffer();
  EditBufferUseCases useCase(eb);
  useCase.lockAllGroups();
  setup();
}
