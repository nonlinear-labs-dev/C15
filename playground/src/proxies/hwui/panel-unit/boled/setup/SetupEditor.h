#pragma once

#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/buttons.h>

class SetupLayout;

class SetupEditor
{
 public:
  SetupEditor();
  virtual ~SetupEditor();

  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers);
  virtual bool onRotary(int inc, ButtonModifiers modifiers);

  void setSetupLayout(SetupLayout* layout);
  void diveUp();

 private:
  SetupLayout* m_setupLayout = nullptr;
};
