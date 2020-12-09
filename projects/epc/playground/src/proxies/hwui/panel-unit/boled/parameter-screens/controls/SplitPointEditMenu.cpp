#include "SplitPointEditMenu.h"
#include <device-settings/Settings.h>
#include <device-settings/SplitPointSyncParameters.h>
#include <Application.h>
#include <sigc++/adaptors/hide.h>
#include <device-settings/SyncSplitSettingUseCases.h>

void SplitPointEditMenu::addActions()
{
  ParameterEditButtonMenu::addActions();

  if(m_syncSetting->get())
  {
    addButton("Unlink Splitp.", [this]() { SyncSplitSettingUseCases::get().disableSyncSetting(); });
  }
  else
  {
    addButton("Link Splitp.", [this]() { SyncSplitSettingUseCases::get().enableSyncSetting(); });
  }
}

SplitPointEditMenu::SplitPointEditMenu(const Rect& rect)
    : ParameterEditButtonMenu(rect)
{
  m_syncSetting = Application::get().getSettings()->getSetting<SplitPointSyncParameters>().get();

  m_syncConnection = Application::get().getSettings()->getSetting<SplitPointSyncParameters>()->onChange(
      sigc::hide(sigc::mem_fun(this, &SplitPointEditMenu::setup)));
}
