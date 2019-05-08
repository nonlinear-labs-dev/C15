#include <proxies/hwui/panel-unit/boled/undo/UndoList.h>
#include <Application.h>
#include <http/UndoScope.h>
#include "UndoListEntry.h"

UndoList::UndoList(const Rect &pos)
    : super(pos)
{
  for(int i = 0; i < 4; i++)
  {
    addControl(new UndoListEntry(Rect(0, i * 16, pos.getWidth(), 16)));
  }
}

UndoList::~UndoList()
{
}

void UndoList::assignTransactions(shared_ptr<UNDO::Transaction> tip)
{
  auto current = Application::get().getUndoScope()->getUndoTransaction();
  auto walker = tip;

  auto it = getControls().rbegin();

  if(auto lastToShow = walker->getDefaultRedoRoute())
  {
    walker = lastToShow;
  }
  else if(auto entry = dynamic_pointer_cast<UndoListEntry>(*it))
  {
    entry->assignTransaction(nullptr, false, false);
    it++;
  }

  for(; it != getControls().rend(); it++)
  {
    auto ctrl = *it;

    if(auto entry = dynamic_pointer_cast<UndoListEntry>(ctrl))
    {
      entry->assignTransaction(walker, walker == tip, walker == current);

      if(walker)
        walker = walker->getPredecessor();
    }
  }
}

bool UndoList::redraw(FrameBuffer &fb)
{
  super::redraw(fb);

  fb.setColor(FrameBuffer::Colors::C128);
  const Rect &r = getPosition();
  fb.drawRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());

  return true;
}
