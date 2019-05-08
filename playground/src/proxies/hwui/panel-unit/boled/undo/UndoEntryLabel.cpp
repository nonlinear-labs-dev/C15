#include <proxies/hwui/panel-unit/boled/undo/UndoEntryLabel.h>

UndoEntryLabel::UndoEntryLabel(const Rect &pos)
    : super(pos)
{
}

UndoEntryLabel::~UndoEntryLabel()
{
}

void UndoEntryLabel::assignTransaction(shared_ptr<UNDO::Transaction> transaction, bool selected, bool current)
{
  setHighlight(current);
}

void UndoEntryLabel::setBackgroundColor(FrameBuffer &fb) const
{
  if(isHighlight())
    fb.setColor(FrameBuffer::Colors::C128);
  else
    super::setBackgroundColor(fb);
}
