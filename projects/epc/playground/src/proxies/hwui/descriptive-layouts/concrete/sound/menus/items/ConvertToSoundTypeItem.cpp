#include <presets/EditBuffer.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <nltools/Types.h>
#include <libundo/undo/Scope.h>
#include "ConvertToSoundTypeItem.h"

ConvertToSoundTypeItem::ConvertToSoundTypeItem(const Rect& rect, SoundType toType)
    : AnimatedGenericItem(nltools::string::concat("Convert to ", toString(toType)), rect, [=] {
      SoundUseCases useCases(Application::get().getPresetManager()->getEditBuffer(),
                             Application::get().getPresetManager());
      switch(toType)
      {
        case SoundType::Single:
        {
          auto selectedVG = Application::get().getHWUI()->getCurrentVoiceGroup();
          useCases.convertToSingle(selectedVG);
          break;
        }
        case SoundType::Split:
        {
          useCases.convertToSplit();
          break;
        }
        case SoundType::Layer:
        {
          useCases.convertToLayer();
          break;
        }
        default:
          break;
      }

      Application::get().getHWUI()->undoableSetFocusAndMode({ UIFocus::Sound, UIMode::Select, UIDetail::Init });
    })
{
}
