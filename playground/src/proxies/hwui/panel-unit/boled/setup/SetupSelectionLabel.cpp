#include <proxies/hwui/panel-unit/boled/setup/SetupSelectionLabel.h>

SetupSelectionLabel::SetupSelectionLabel(const Glib::ustring &text)
    : SetupLabel(text, Rect(0, 0, 0, 0))
{
}

SetupSelectionLabel::~SetupSelectionLabel()
{
}

bool SetupSelectionLabel::redraw(FrameBuffer &fb)
{
  return SetupLabel::redraw(fb);
}

void SetupSelectionLabel::drawBackground(FrameBuffer &fb)
{
}
