#include "DisplayEditor.h"

DisplayEditor::DisplayEditor()
    : ControlWithChildren(Rect(0, 0, 0, 0))
    , SetupEditor()
    , m_entrys{}
{
}

std::pair<LeftAlignedLabel*, LeftAlignedLabel*>
    DisplayEditor::addEntry(const Glib::ustring& key, const Glib::ustring& name, const Glib::ustring& value)
{
  m_order.push_back(key);
  m_entrys[key]
      = std::make_pair(new LeftAlignedLabel(name, Rect(0, 0, 50, 12)), new LeftAlignedLabel(value, Rect(0, 0, 70, 12)));
  addControl(m_entrys[key].first);
  addControl(m_entrys[key].second);
  return m_entrys[key];
};

void DisplayEditor::doLayout()
{
  Control::setPosition(Rect(0, 16, 256, 96));
  int y = 3;
  for(const auto& key : m_order)
  {
    auto label = m_entrys[key].first;
    auto valueLabel = m_entrys[key].second;
    auto height = std::max(label->getHeight(), valueLabel->getHeight());
    Rect labelpos(5, y, 50, height);
    Rect valuepos(55, y, 196, height);
    label->setPosition(labelpos);
    valueLabel->setPosition(valuepos);
    y += height;
  }
}

bool DisplayEditor::redraw(FrameBuffer& fb)
{
  return ControlWithChildren::redraw(fb);
}

void DisplayEditor::drawBackground(FrameBuffer& fb)
{
  fb.setColor(FrameBuffer::Colors::C43);
  fb.fillRect(getPosition());
  ControlWithChildren::drawBackground(fb);
}
