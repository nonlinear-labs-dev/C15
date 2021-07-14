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
  explicit PedalEditor(std::shared_ptr<PedalType> m);
  ~PedalEditor() override;
  void setPosition(const Rect& r) override;
  void incSetting(int inc) override;
  const std::vector<Glib::ustring>& getDisplayStrings() const override;
  int getSelectedIndex() const override;

 private:
  std::shared_ptr<PedalType> m_mode;
};
