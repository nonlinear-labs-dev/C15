#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include "MenuEditor.h"

class Setting;
class PedalType;

class PedalEditor : public MenuEditor
{
 private:
  typedef MenuEditor super;

 public:
  PedalEditor(std::shared_ptr<PedalType> m);
  virtual ~PedalEditor();

  virtual void incSetting(int inc) override;
  virtual const std::vector<Glib::ustring> &getDisplayStrings() const override;
  virtual int getSelectedIndex() const override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  void setPosition(const Rect& r) override;

 private:
  void load();

 protected:
  void updateOnSettingChanged() override;

 private:
  int m_selected;
  std::shared_ptr<PedalType> m_mode;

  friend class PedalMenuEditorEntry;
};
