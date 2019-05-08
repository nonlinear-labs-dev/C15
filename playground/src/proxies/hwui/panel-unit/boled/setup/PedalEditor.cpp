#include <Application.h>
#include <device-settings/PedalType.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/panel-unit/boled/setup/MenuEditorEntry.h>
#include <proxies/hwui/panel-unit/boled/setup/PedalEditor.h>

PedalEditor::PedalEditor(shared_ptr<PedalType> m)
    : m_mode(m)
{
  m_mode->onChange(mem_fun(this, &PedalEditor::onSettingChanged));
}

PedalEditor::~PedalEditor()
{
}

void PedalEditor::incSetting(int inc)
{
  m_mode->inc(inc);
}

const vector<ustring> &PedalEditor::getDisplayStrings() const
{
  return m_mode->enumToDisplayString();
}

int PedalEditor::getSelectedIndex() const
{
  return (int) m_mode->get();
}
