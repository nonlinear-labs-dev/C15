#include <Application.h>
#include <http/UndoScope.h>
#include <presets/Preset.h>
#include <presets/Bank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/EditPresetInfoLayout.h>
#include <use-cases/PresetUseCases.h>

EditPresetInfoLayout::EditPresetInfoLayout()
    : super()
{
  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
  {
    m_currentPreset = bank->getSelectedPreset();
  }
}

void EditPresetInfoLayout::commit(const Glib::ustring &comment)
{
  if(m_currentPreset)
  {
    PresetUseCases useCase(m_currentPreset);
    useCase.setComment(comment);
  }
}

Glib::ustring EditPresetInfoLayout::getInitialText() const
{
  if(m_currentPreset)
    return m_currentPreset->getComment();

  return "";
}
