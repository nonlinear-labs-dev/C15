#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/InfoLayout.h>

class ParameterInfoLayout : public InfoLayout
{
 private:
  typedef InfoLayout super;

 public:
  ParameterInfoLayout();

  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

 protected:
  void addHeadline() override;
  void addModuleCaption() override;
  void addInfoLabel() override;
  Scrollable *createScrollableContent() override;
};
