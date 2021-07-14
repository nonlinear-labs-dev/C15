#pragma once

#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/panel-unit/ButtonReceiver.h>
#include "proxies/hwui/descriptive-layouts/concrete/menu/menu-items/MenuItem.h"
#include "proxies/hwui/descriptive-layouts/concrete/menu/menu-overlays/MenuOverlay.h"

class ScrollMenu : public ControlWithChildren, public ButtonReceiver
{
 public:
  explicit ScrollMenu(const Rect &r);
  bool onButton(Buttons i, bool down, ButtonModifiers mod) override;

  void doLayout();
  void scroll(int direction);

  virtual void init() = 0;

  template <class T, class... tArgs> void addItem(tArgs... args)
  {
    m_items.emplace_back(addControl(new T(args...)));
    doLayout();
  }

 protected:
  MenuOverlay *m_overlay = nullptr;

  std::vector<MenuItem *> m_items;
  int m_selectedItem { 0 };

  bool handleScrolling(const Buttons &i, bool down);
  bool onButtonOverlay(const Buttons &i, bool down, const ButtonModifiers &mod);
  bool onSelectedItemButtonHandler(const Buttons &i, bool down, const ButtonModifiers &mod);
  virtual int getDefaultItemHeight() const;
};
