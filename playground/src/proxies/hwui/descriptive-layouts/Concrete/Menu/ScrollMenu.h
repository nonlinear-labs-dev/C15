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
  }

  void scroll(int direction);

 protected:
  ControlWithChildren* m_overlay;

  BasicItem* m_selectedControl = nullptr;
  std::vector<BasicItem*> m_items;
};
