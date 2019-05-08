#include <proxies/hwui/panel-unit/boled/setup/MenuEditorEntry.h>

MenuEditorEntry::MenuEditorEntry()
    : super(Rect(0, 0, 0, 0))
{
}

MenuEditorEntry::~MenuEditorEntry()
{
}

void MenuEditorEntry::assign(const vector<Glib::ustring> &entries, int idx, bool selected)
{
  if(idx < 0 || idx >= entries.size())
    setText("", 0);
  else
    setText(entries[idx], 0);

  setHighlight(selected);
}

void MenuEditorEntry::setFontColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBuffer::C255);
  else
    super::setFontColor(fb);
}

void MenuEditorEntry::setBackgroundColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBuffer::C103);
  else
    super::setBackgroundColor(fb);
}

Font::Justification MenuEditorEntry::getJustification() const
{
  return Font::Justification::Center;
}
