#include <Application.h>
#include <device-settings/PedalType.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/panel-unit/boled/setup/MenuEditorEntry.h>
#include <proxies/hwui/panel-unit/boled/setup/PedalEditor.h>
#include <utility>

PedalEditor::PedalEditor(std::shared_ptr<PedalType> m)
    : m_mode(std::move(m))
{
  m_mode->onChange(mem_fun(this, &PedalEditor::onSettingChanged));
}

PedalEditor::~PedalEditor() = default;

void PedalEditor::incSetting(int inc)
{
  m_mode->incDec(inc, false);
}

const std::vector<Glib::ustring>& PedalEditor::getDisplayStrings() const
{
  return m_mode->getDisplayStrings();
}

int PedalEditor::getSelectedIndex() const
{
  auto v = getDisplayStrings();
  auto it = std::find(v.begin(), v.end(), m_mode->getDisplayString());
  if(it != v.end())
    return std::distance(v.begin(), it);
  return 0;
}

void PedalEditor::setPosition(const Rect& r)
{
  static const Rect menuEditorPosition(129, 16, 126, 48);
  Control::setPosition(menuEditorPosition);

  int y = 0;
  int h = 12;
  int w = menuEditorPosition.getWidth();
  int x = (menuEditorPosition.getWidth() - w) / 2;

  for(auto& c : getControls())
  {
    c->setPosition(Rect(x, y, w, h));
    y += h;
  }
}
