#pragma once

#include <proxies/hwui/Layout.h>
#include <proxies/hwui/HWUIEnums.h>

class Rect;

class ScrollArea;
class Scrollable;

class InfoLayout : public Layout
{
 private:
  typedef Layout super;

 public:
  InfoLayout();

  void init() override;

 protected:
  bool onRotary(int inc, ButtonModifiers modifiers) override;
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;

  virtual void addModuleCaption() = 0;
  virtual void addHeadline() = 0;
  virtual void addInfoLabel() = 0;
  virtual Scrollable* createScrollableContent() = 0;
  virtual Rect getScrollableAreaRect() const;

  void scrollToMax();

 private:
  ScrollArea* m_scrollArea = nullptr;
};
