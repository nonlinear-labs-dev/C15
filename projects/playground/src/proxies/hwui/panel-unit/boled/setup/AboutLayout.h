#pragma once

#include <proxies/hwui/Layout.h>

class ScrollArea;

class AboutLayout : public Layout
{
  typedef Layout super;

 public:
  AboutLayout();
  virtual ~AboutLayout();

  virtual bool onRotary(int inc, ButtonModifiers modifiers) override;
  virtual bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

 private:
  ScrollArea *m_scrollArea = nullptr;
};
