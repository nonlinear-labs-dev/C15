#include "MiniParameterLabel.h"

MiniParameterLabel::MiniParameterLabel(const Glib::ustring &text, const Rect &pos)
    : super(text, pos)
    , m_selected(false)
{
}

MiniParameterLabel::~MiniParameterLabel()
{
}

void MiniParameterLabel::setSelected(bool selected)
{
  if(m_selected != selected)
  {
    m_selected = selected;
    setDirty();
  }
}

bool MiniParameterLabel::isSelected() const
{
  return m_selected;
}

bool MiniParameterLabel::redraw(FrameBuffer &fb)
{
  auto r = getPosition();

  if(m_selected)
  {
    fb.setColor(FrameBuffer::Colors::C103);
    fb.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight());
  }

  super::redraw(fb);
  return true;
}

void MiniParameterLabel::setFontColor(FrameBuffer &fb) const
{
  if(m_selected)
    if(isHighlight())
      fb.setColor(FrameBuffer::Colors::C255);
    else
      fb.setColor(FrameBuffer::Colors::C128);
  else
    fb.setColor(FrameBuffer::Colors::C128);
}
