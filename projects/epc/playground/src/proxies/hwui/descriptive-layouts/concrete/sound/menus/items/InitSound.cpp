#include <Application.h>
#include "InitSound.h"
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <libundo/undo/Scope.h>

InitSound::InitSound(const Rect& rect)
    : AnimatedGenericItem("Init Sound", rect, [] {
      auto pm = Application::get().getPresetManager();
      SoundUseCases useCase { pm->getEditBuffer(), pm };
      useCase.initSound();
      auto hwui = Application::get().getHWUI();
      hwui->setFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Init });
    })
{
}

InitPart::InitPart(const Rect& r)
    : AnimatedGenericItem("Init Part", r, []() {
      auto pm = Application::get().getPresetManager();
      SoundUseCases useCases { pm->getEditBuffer(), pm };
      auto currentPart = Application::get().getHWUI()->getCurrentVoiceGroup();
      useCases.initPart(currentPart);
      auto hwui = Application::get().getHWUI();
      hwui->setFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Init });
    })
{
}
