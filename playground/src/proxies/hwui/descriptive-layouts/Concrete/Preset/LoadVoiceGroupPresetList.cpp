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
  if(auto preset = getPresetAtSelected()) {
    Application::get().getPresetManager()->getEditBuffer()->loadCurrentVG(preset);
    Application::get().getHWUI()->setFocusAndMode(UIDetail::Init);
  }
}
