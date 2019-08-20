#pragma once

#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/panel-unit/ButtonReceiver.h>
#include "BasicItem.h"

class ScrollMenu : public ControlWithChildren, public ButtonReceiver
{
public:
  bool onButton(Buttons i, bool down, ButtonModifiers mod) override;

public:
  ScrollMenu();

  virtual void init() = 0;

  template <class T, class... tArgs> void addItem(tArgs... args)
  {
    m_items.emplace_back(addControl(new T(args...)));
    doLayout();
  }

  void doLayout();

  void scroll(int direction);

 protected:
  std::vector<BasicItem*> m_items;
  int m_selectedItem{0};
  const int m_numPlaces;
};
