#include <Application.h>
#include <http/UndoScope.h>
#include <presets/Preset.h>
#include <presets/PresetBank.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/HWUI.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/RenamePresetLayout.h>

RenamePresetLayout::RenamePresetLayout(tCommitCB commitCB, tCancelCB cancelCB)
    : super()
    , m_commitCB(commitCB)
    , m_cancelCB(cancelCB)

{
  if(auto bank = Application::get().getPresetManager()->getSelectedBank())
  {
    const auto &uuid = bank->getSelectedPreset();
    m_currentPreset = bank->getPreset(uuid);
  }
}

void RenamePresetLayout::commit(const Glib::ustring &newName)
{
  if(m_commitCB)
    m_commitCB(newName);
}

void RenamePresetLayout::cancel()
{
  if(m_cancelCB)
    m_cancelCB();
}

Glib::ustring RenamePresetLayout::getInitialText() const
{
  if(m_currentPreset)
    return m_currentPreset->getName();

  return "";
}
