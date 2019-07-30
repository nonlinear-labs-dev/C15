#include "SingeltonShortcuts.h"
#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/LayoutMode.h>

#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

namespace SiSc
{
  LayoutVersionMode getLayoutSetting()
  {
    return Application::get().getSettings()->getSetting<LayoutMode>()->get();
  }
}

namespace SiSc::EB
{
  const Parameter* getCurrentParameter() {
    return Application::get().getPresetManager()->getEditBuffer()->getSelected();
  }
}
