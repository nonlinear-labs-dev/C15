#pragma once

#include "MenuEditor.h"

class Setting;

//remove this and use enumsettingeditor
class SignalFlowIndicatorEditor : public MenuEditor
{
 private:
  typedef MenuEditor super;

 public:
  SignalFlowIndicatorEditor();
  ~SignalFlowIndicatorEditor() override = default;

  void incSetting(int inc) override;
  const std::vector<Glib::ustring> &getDisplayStrings() const override;
  int getSelectedIndex() const override;
};
