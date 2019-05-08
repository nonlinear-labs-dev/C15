#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

SetupLabel::SetupLabel(const Glib::ustring &text, const Rect &pos)
    : Label(text, pos)
{
}

SetupLabel::SetupLabel(const Rect &pos)
    : Label(pos)
{
}

SetupLabel::~SetupLabel()
{
}

Font::Justification SetupLabel::getJustification() const
{
  return Font::Justification::Left;
}

bool SetupLabel::redraw(FrameBuffer &fb)
{
  return Label::redraw(fb);
}

int SetupLabel::getXOffset() const
{
  return 4;
}
