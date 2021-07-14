#pragma once

#include "MenuEditor.h"

class Setting;

class VelocityEditor : public MenuEditor
{
 private:
  typedef MenuEditor super;

 public:
  VelocityEditor();
  ~VelocityEditor() override;

  void incSetting(int inc) override;
  const std::vector<Glib::ustring> &getDisplayStrings() const override;
  int getSelectedIndex() const override;
};
