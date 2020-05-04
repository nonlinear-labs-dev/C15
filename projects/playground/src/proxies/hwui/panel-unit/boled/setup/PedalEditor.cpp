#include <Application.h>
#include <device-settings/PedalType.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/panel-unit/boled/setup/MenuEditorEntry.h>
#include <proxies/hwui/panel-unit/boled/setup/PedalEditor.h>
#include <proxies/hwui/FrameBuffer.h>

class PedalMenuEditorEntry : public MenuEditorEntry
{
 public:
  PedalMenuEditorEntry(PedalEditor* parent, int i)
      : m_parent(parent)
      , m_index { i }
  {
  }

  bool redraw(FrameBuffer& fb) override
  {
    auto ret = SetupLabel::redraw(fb);

    /*if(isHighlight())
    {
      fb.setColor(FrameBufferColors::C179);
      fb.drawRect(getPosition());
    }*/

    return ret;
  }

 protected:
  void setFontColor(FrameBuffer& fb) const override
  {
    if((int) m_parent->m_mode->get() == m_index || isHighlight())
    {
      fb.setColor(FrameBufferColors::C255);
    }
    else
    {
      Label::setFontColor(fb);
    }
  }

  void setBackgroundColor(FrameBuffer& fb) const override
  {
    if((int) m_parent->m_mode->get() == m_index)
    {
      fb.setColor(FrameBufferColors::C179);
    }
    else
    {
      fb.setColor(FrameBufferColors::C43);
    }
  }

 private:
  const int m_index;
  PedalEditor* m_parent;
};

PedalEditor::PedalEditor(std::shared_ptr<PedalType> m)
    : m_mode(m)
{
  m_selected = (int) m_mode->get();
  m_mode->onChange(mem_fun(this, &PedalEditor::onSettingChanged));

  clear();
  for(int i = 0; i < 4; i++)
  {
    addControl(new PedalMenuEditorEntry(this, i));
  }
}

PedalEditor::~PedalEditor()
{
}

bool PedalEditor::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  if(down)
  {
    switch(i)
    {
      case Buttons::BUTTON_ENTER:
        load();
        return true;
    }
  }

  return MenuEditor::onButton(i, down, modifiers);
}

void PedalEditor::load()
{
  m_mode->set(static_cast<PedalTypes>(m_selected));
  setDirty();
}

void PedalEditor::incSetting(int inc)
{
  m_selected += inc;

  if(m_selected >= m_mode->enumToDisplayString().size())
    m_selected = 0;
  if(m_selected < 0)
    m_selected = m_mode->enumToDisplayString().size() - 1;

  onSettingChanged(m_mode.get());
}

const std::vector<Glib::ustring>& PedalEditor::getDisplayStrings() const
{
  return m_mode->enumToDisplayString();
}

void PedalEditor::updateOnSettingChanged()
{
  auto& entries = getDisplayStrings();
  const auto selectedIndex = getSelectedIndex();
  auto assignIndex = 0;

  for(auto& c : getControls())
  {
    if(auto e = std::dynamic_pointer_cast<MenuEditorEntry>(c))
    {
      e->assign(entries, assignIndex, assignIndex == selectedIndex);
      assignIndex++;
    }
  }
}

int PedalEditor::getSelectedIndex() const
{
  return m_selected;
}
