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
  virtual void addHeadline() override;
  virtual void addModuleCaption() override;
  virtual void addInfoLabel() override;
  virtual Scrollable *createScrollableContent() override;
};
