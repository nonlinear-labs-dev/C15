#pragma once
#include "BasicItem.h"
#include "proxies/hwui/descriptive-layouts/Concrete/Menu/MenuOverlay/ScrollMenuOverlay.h"

class EditorItem : public BasicItem {
public:
  bool canEnter() override;

  template <class tCaption>
  explicit EditorItem(tCaption cap) : BasicItem(cap) {
  }

  Rect getStandartOverlayRect();

  virtual ScrollMenuOverlay* createOverlay() = 0;

  void doAction() override;
};
