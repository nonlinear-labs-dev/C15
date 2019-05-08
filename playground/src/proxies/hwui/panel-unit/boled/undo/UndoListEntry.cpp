#include "UndoListEntry.h"
#include "PrevUndoTransactionSiblingControl.h"
#include "NextUndoTransactionSiblingControl.h"
#include "UndoTransactionNameControl.h"

UndoListEntry::UndoListEntry(const Rect &pos)
    : super(pos)
{
  const int siblingsWidth = 8;
  const int border = 2;
  const int height = 12;

  addControl(new PrevUndoTransactionSiblingControl(Rect(border, border, siblingsWidth, height)));
  addControl(new UndoTransactionNameControl(
      Rect(border + siblingsWidth, border, pos.getWidth() - 2 * border - 2 * siblingsWidth, height)));
  addControl(new NextUndoTransactionSiblingControl(
      Rect(pos.getWidth() - border - siblingsWidth, border, siblingsWidth, height)));
}

UndoListEntry::~UndoListEntry()
{
}

bool UndoListEntry::redraw(FrameBuffer &fb)
{
  super::redraw(fb);

  if(m_selected)
  {
    fb.setColor(FrameBuffer::Colors::C179);
    const Rect &r = getPosition();
    fb.drawRect(r.getLeft() + 1, r.getTop(), r.getWidth() - 2, r.getHeight());

    if(m_current)
    {
      fb.setColor(FrameBuffer::Colors::C43);
      const Rect &r = getPosition();
      fb.drawRect(r.getLeft() + 2, r.getTop() + 1, r.getWidth() - 4, r.getHeight() - 2);
    }
  }

  return true;
}

void UndoListEntry::assignTransaction(shared_ptr<UNDO::Transaction> transaction, bool selected, bool current)
{
  forEach([&](shared_ptr<Control> ptr) {
    if(auto p = dynamic_pointer_cast<UndoTransactionClient>(ptr))
      p->assignTransaction(transaction, selected, current);
  });

  if(m_selected != selected)
  {
    m_selected = selected;
    setDirty();
  }

  if(m_current != current)
  {
    m_current = current;
    setDirty();
  }
}
