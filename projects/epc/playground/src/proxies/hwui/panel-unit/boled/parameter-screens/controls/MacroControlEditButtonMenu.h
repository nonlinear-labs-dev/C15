#pragma once

#include "proxies/hwui/controls/ButtonMenu.h"

class PresetBank;

class MacroControlEditButtonMenu : public ButtonMenu
{
 private:
  typedef ButtonMenu super;

 public:
  explicit MacroControlEditButtonMenu(const Rect &rect);
  ~MacroControlEditButtonMenu() override;

  void selectButton(size_t i) override;

 private:
  void setup();
  void onGroupChanged();

  void editInfo();
  void rename();

  void unlockGroup();
  void lockGroup();
  void unlockAll();
  void lockAll();

  static int s_lastAction;
};
