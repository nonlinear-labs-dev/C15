#pragma once

#include "MenuEditor.h"

class Setting;

class VelocityEditor : public MenuEditor
{
 private:
  typedef MenuEditor super;

 public:
  VelocityEditor();
  virtual ~VelocityEditor();

  void incSetting(int inc) override;
  const vector<ustring> &getDisplayStrings() const override;
  int getSelectedIndex() const override;
};
