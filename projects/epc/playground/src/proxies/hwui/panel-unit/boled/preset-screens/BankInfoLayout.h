#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/InfoLayout.h>

class Bank;

class BankInfoLayout : public InfoLayout
{
 private:
  typedef InfoLayout super;

 public:
  BankInfoLayout();
  ~BankInfoLayout() override;

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

 protected:
  void addModuleCaption() override;
  void addHeadline() override;
  void addInfoLabel() override;
  Scrollable* createScrollableContent() override;
};
