#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>

class SetupSelectionEntries : public ControlWithChildren
{
 public:
  explicit SetupSelectionEntries(const Rect &pos);
  ~SetupSelectionEntries() override;

  void addEntry(Control *s);
  void finish(bool selectMode);

 private:
  typedef std::list<Control *> tEntries;

  void assignDownwards(tEntries::iterator entryIt, tControls::const_iterator controlIt, bool selectMode);

  tEntries m_entries;
};
