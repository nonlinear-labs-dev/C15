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
      auto scope = Application::get().getPresetManager()->getUndoScope().startTransaction(
          nltools::string::concat("Convert Sound to ", toString(toType)));
      auto transaction = scope->getTransaction();
      auto currentVoiceGroup = Application::get().getHWUI()->getCurrentVoiceGroup();

      switch(toType)
      {
        case SoundType::Single:
          Application::get().getPresetManager()->getEditBuffer()->undoableConvertToSingle(transaction,
                                                                                          currentVoiceGroup);
          break;

        case SoundType::Split:
        case SoundType::Layer:
          Application::get().getPresetManager()->getEditBuffer()->undoableConvertToDual(transaction, toType);
          break;

        case SoundType::Invalid:
          assert(false);
      }

      Application::get().getHWUI()->undoableSetFocusAndMode(transaction,
                                                            { UIFocus::Sound, UIMode::Select, UIDetail::Init });
    })
{
}
