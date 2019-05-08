#include "ViewEntries.h"
#include "SetupSelectionEntry.h"

const int entryHeight = 12;

ViewEntries::ViewEntries(const Rect &pos)
    : ControlWithChildren(pos)
{
}

ViewEntries::~ViewEntries()
{
  for(auto p : m_entries)
    if(p.first)
      delete p.first;
}

void ViewEntries::addEntry(Control *child, bool focussed)
{
  m_entries.push_back(make_pair(child, focussed));
}

void ViewEntries::assignDownwards(tEntries::iterator entryIt, int idx)
{
  if(entryIt != m_entries.end() && idx < 4)
  {
    auto c = addControl(entryIt->first);
    c->setHighlight(entryIt->second);
    entryIt->first = nullptr;
    c->setPosition(Rect(0, idx * entryHeight, getPosition().getWidth(), entryHeight));
    assignDownwards(++entryIt, idx + 1);
  }
}

void ViewEntries::finish()
{
  for(auto it = m_entries.begin(); it != m_entries.end(); it++)
  {
    if(it->second)
    {
      if(it == m_entries.begin())
      {
        assignDownwards(it, 1);
      }
      else
      {
        assignDownwards(--it, 0);
      }
      return;
    }
  }
}
