#include "SingeltonShortcuts.h"
#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/LayoutMode.h>

#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>

#include "proxies/hwui/HWUI.h"
#include "proxies/hwui/panel-unit/boled/BOLED.h"

namespace SiSc
{
  LayoutVersionMode getLayoutSetting()
  {
    return Application::get().getSettings()->getSetting<LayoutMode>()->get();
  }

  namespace EB
  {
    const Parameter *getCurrentParameter()
    {
      return Application::get().getPresetManager()->getEditBuffer()->getSelected();
    }
  }

  namespace HWUI
  {
    void bruteForce()
    {
      Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().bruteForce();
    }
  }
}