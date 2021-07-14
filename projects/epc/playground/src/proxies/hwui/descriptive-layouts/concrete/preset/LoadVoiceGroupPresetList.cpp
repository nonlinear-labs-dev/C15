#include "LoadVoiceGroupPresetList.h"
#include "Application.h"
#include "presets/PresetManager.h"
#include "presets/EditBuffer.h"
#include "proxies/hwui/HWUI.h"

LoadVoiceGroupPresetList::LoadVoiceGroupPresetList(const Point &p)
    : GenericPresetList(p)
{
  signalChanged();
}

void LoadVoiceGroupPresetList::action()
{
  if(auto preset = getPresetAtSelected())
  {
    EditBufferUseCases useCase(Application::get().getPresetManager()->getEditBuffer());
    useCase.undoableLoadToPart(preset, VoiceGroup::I, Application::get().getHWUI()->getCurrentVoiceGroup());
    Application::get().getHWUI()->setFocusAndMode(UIDetail::Init);
  }
}
