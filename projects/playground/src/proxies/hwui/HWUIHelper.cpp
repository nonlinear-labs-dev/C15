#include <Application.h>
#include <proxies/hwui/HWUI.h>
#include "HWUIHelper.h"
#include "HWUIEnums.h"

bool HWUIHelper::isLoadToPartActive()
{
  auto hwui = Application::get().getHWUI();
  return hwui->getFocusAndMode().detail == UIDetail::LoadToPart;
}
