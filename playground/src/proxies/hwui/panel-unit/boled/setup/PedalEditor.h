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
  PedalEditor(shared_ptr<PedalType> m);
  virtual ~PedalEditor();

  virtual void incSetting(int inc) override;
  virtual const vector<ustring> &getDisplayStrings() const override;
  virtual int getSelectedIndex() const override;

 private:
  shared_ptr<PedalType> m_mode;
};
