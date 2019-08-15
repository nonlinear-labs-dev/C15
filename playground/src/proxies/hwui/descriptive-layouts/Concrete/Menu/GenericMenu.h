#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/ButtonReceiver.h>
#include "BasicItem.h"
#include "GenericMenuEditor.h"

class GenericMenu : public ControlWithChildren, public ButtonReceiver
{
 public:
  GenericMenu(const Rect& r);
  virtual void init();

  bool onButton(Buttons i, bool down, ButtonModifiers mod) override;

  void selectNext();
  void selectPrev();
  void enter();
  void diveUp();

  virtual bool allowWrapAround() const;

  bool redraw(FrameBuffer& fb) override;



 protected:
  void bruteForce();

  std::stack<std::unique_ptr<GenericMenuEditor>> m_overlays;
  std::vector<std::unique_ptr<BasicItem>> m_items;

  void sanitizeIndex();
  int m_selection;

  void bruteForceList() const;

  void hideAllControls() const;

  Rect calculatePosition(BasicItem& item) const;
};
