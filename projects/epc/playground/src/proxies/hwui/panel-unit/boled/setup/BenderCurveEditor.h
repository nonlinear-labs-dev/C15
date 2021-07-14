#pragma once

#include "MenuEditor.h"

class Setting;

class BenderCurveEditor : public MenuEditor
{
 private:
  typedef MenuEditor super;

 public:
  BenderCurveEditor();
  ~BenderCurveEditor() override;

  void incSetting(int inc) override;
  const std::vector<Glib::ustring> &getDisplayStrings() const override;
  int getSelectedIndex() const override;
};
