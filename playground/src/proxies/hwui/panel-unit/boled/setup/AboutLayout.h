#pragma once

#include <proxies/hwui/DFBLayout.h>

class ScrollArea;

class AboutLayout : public DFBLayout
{
  typedef DFBLayout super;

 public:
  AboutLayout();
  virtual ~AboutLayout();

  virtual bool onRotary(int inc, ButtonModifiers modifiers) override;
  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

 private:
  ScrollArea *m_scrollArea = nullptr;
};
