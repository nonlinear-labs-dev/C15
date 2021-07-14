#pragma once

#include "MenuEditor.h"

class Setting;

class PresetGlitchSuppressionEditor : public MenuEditor
{
 private:
  typedef MenuEditor super;

 public:
  PresetGlitchSuppressionEditor();
  ~PresetGlitchSuppressionEditor() override;

  void incSetting(int inc) override;
  const std::vector<Glib::ustring> &getDisplayStrings() const override;
  int getSelectedIndex() const override;
};
