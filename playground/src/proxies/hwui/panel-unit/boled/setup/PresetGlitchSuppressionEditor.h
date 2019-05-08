#pragma once

#include "MenuEditor.h"

class Setting;

class PresetGlitchSuppressionEditor : public MenuEditor
{
 private:
  typedef MenuEditor super;

 public:
  PresetGlitchSuppressionEditor();
  virtual ~PresetGlitchSuppressionEditor();

  void incSetting(int inc) override;
  const vector<ustring> &getDisplayStrings() const override;
  int getSelectedIndex() const override;
};
