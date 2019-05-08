#include "SetupSelectionEntries.h"
#include "SetupSelectionEntry.h"

SetupSelectionEntries::SetupSelectionEntries(const Rect &pos)
    : ControlWithChildren(pos)
{
  int y = 0;
  int height = 12;
  int margin = 0;

  for(int i = 0; i < 4; i++)
  {
    addControl(new SetupSelectionEntry(i, Rect(margin, i * height, pos.getWidth() - margin, height)));
  }
}

SetupSelectionEntries::~SetupSelectionEntries()
{
  for(auto e : m_entries)
    if(e)
      delete e;
}

void SetupSelectionEntries::addEntry(Control *s)
{
  m_entries.push_back(s);
}

void SetupSelectionEntries::assignDownwards(tEntries::iterator entryIt, tControls::const_iterator controlIt,
                                            bool selectMode)
{
  if(entryIt != m_entries.end() && controlIt != getControls().end())
  {
    auto p = dynamic_pointer_cast<SetupSelectionEntry>(*controlIt);
    p->assign(*entryIt, selectMode);
    *entryIt = nullptr;
    assignDownwards(++entryIt, ++controlIt, selectMode);
  }
}

void SetupSelectionEntries::finish(bool selectMode)
{
  for(auto it = m_entries.begin(); it != m_entries.end(); it++)
  {
    if((*it)->isHighlight())
    {
      auto controlIteratorToFirst = getControls().begin();

      if(it == m_entries.begin())
      {
        controlIteratorToFirst++;
        assignDownwards(it, controlIteratorToFirst, selectMode);
      }
      else
      {
        assignDownwards(--it, controlIteratorToFirst, selectMode);
      }
      return;
    }
  }
}
