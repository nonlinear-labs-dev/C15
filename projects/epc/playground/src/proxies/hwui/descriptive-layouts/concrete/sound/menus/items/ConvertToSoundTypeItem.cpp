#include <presets/EditBuffer.h>
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <nltools/Types.h>
#include <libundo/undo/Scope.h>
#include "ConvertToSoundTypeItem.h"
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SoundEditInfoLayout.h>

ConvertToSoundTypeItem::ConvertToSoundTypeItem(const Rect& rect, SoundType toType)
    : AnimatedGenericItem(
        nltools::string::concat("Convert to ", toString(toType)), rect,
        [=] {
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
    , m_convertToType { toType }
{
}

Control* ConvertToSoundTypeItem::createInfo()
{
  switch(m_convertToType)
  {
    default:
    case SoundType::Single:
      return new SoundEditInfoLayout(SetupInfoEntries::ConvertToSingle);
    case SoundType::Split:
      return new SoundEditInfoLayout(SetupInfoEntries::ConvertToSplit);
    case SoundType::Layer:
      return new SoundEditInfoLayout(SetupInfoEntries::ConvertToLayer);
  }
}
