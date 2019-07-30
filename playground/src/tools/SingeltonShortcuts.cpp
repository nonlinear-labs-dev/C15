#include "SingeltonShortcuts.h"
#include <Application.h>
#include <device-settings/Settings.h>
#include <device-settings/LayoutMode.h>

namespace SiSc
{
  LayoutVersionMode getLayoutSetting()
  {
    return Application::get().getSettings()->getSetting<LayoutMode>()->get();
  }
}
