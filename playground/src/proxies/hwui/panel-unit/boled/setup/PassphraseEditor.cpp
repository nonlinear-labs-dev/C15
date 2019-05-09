#include <Application.h>
#include <device-settings/Passphrase.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/setup/PassphraseEditor.h>

PassphraseEditor::PassphraseEditor()
{
  updateOnSettingChanged();
}

PassphraseEditor::~PassphraseEditor()
{
}

void PassphraseEditor::incSetting(int inc)
{
  m_selection = m_selection + inc;
  m_selection = std::min(m_selection, 1);
  m_selection = std::max(m_selection, 0);
  updateOnSettingChanged();
}

const std::vector<ustring> &PassphraseEditor::getDisplayStrings() const
{
  static std::vector<ustring> r = { "Cancel", "Create New" };
  return r;
}

int PassphraseEditor::getSelectedIndex() const
{
  return m_selection;
}

bool PassphraseEditor::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(MenuEditor::onButton(i, down, modifiers))
    return true;

  if(down && i == Buttons::BUTTON_ENTER)
  {
    if(m_selection == 1)
    {
      Application::get().getSettings()->getSetting<Passphrase>()->dice();
    }
  }
  return false;
}
