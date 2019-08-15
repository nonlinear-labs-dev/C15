#include "LoadVoiceGroupPresetList.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "proxies/hwui/HWUI.h"

LoadVoiceGroupPresetList::LoadVoiceGroupPresetList(const Point &p)
    : GenericPresetList(p)
{
}

void LoadVoiceGroupPresetList::action()
{
  Application::get().getPresetManager()->getEditBuffer()->loadCurrentVG(getPresetAtSelected());
  Application::get().getHWUI()->setFocusAndMode(UIDetail::Init);
}
