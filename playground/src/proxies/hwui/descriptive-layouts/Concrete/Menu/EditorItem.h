#pragma once
#include "BasicItem.h"

class GenericMenuEditor;

class EditorItem : public BasicItem {
public:
  bool canEnter() override;

  template <class tCaption>
  explicit EditorItem(tCaption cap) : BasicItem(cap) {
  }

  virtual GenericMenuEditor* createEditor() = 0;
};
