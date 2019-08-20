#pragma once
#include "BasicItem.h"
#include "ScrollMenuOverlay.h"

class EditorItem : public BasicItem {
public:
  bool canEnter() override;

  template <class tCaption>
  explicit EditorItem(tCaption cap) : BasicItem(cap) {
  }

  virtual ScrollMenuOverlay* createEditor() = 0;
};
