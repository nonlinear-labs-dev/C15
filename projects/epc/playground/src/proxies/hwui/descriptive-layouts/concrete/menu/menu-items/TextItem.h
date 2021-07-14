#pragma once

#include "BasicItem.h"

class TextItem : public BasicItem
{
 public:
  TextItem(const std::string& text, const Rect& r);
  bool canEnter() override;

  void doAction() override;
};
