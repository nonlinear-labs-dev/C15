#pragma once

#include "proxies/hwui/controls/ButtonMenu.h"

class PresetBank;

class MacroControlEditButtonMenu : public ButtonMenu
{
 private:
  typedef ButtonMenu super;

 public:
  MacroControlEditButtonMenu(const Rect &rect);
  virtual ~MacroControlEditButtonMenu();

  void selectButton(size_t i) override;

 private:
  void setup();
  void onGroupChanged();

  void editInfo();
  void rename();
  void reset();
  void smoothing();

  void unlockGroup();
  void lockGroup();
  void unlockAll();
  void lockAll();

  static int s_lastAction;
};
