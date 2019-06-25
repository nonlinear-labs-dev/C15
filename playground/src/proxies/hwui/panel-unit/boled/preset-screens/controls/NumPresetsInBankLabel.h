#pragma once

#include "PresetLabel.h"

class Uuid;

class NumPresetsInBankLabel : public Label
{
 private:
  typedef Label super;

 public:
  NumPresetsInBankLabel(const Rect &pos);
  virtual ~NumPresetsInBankLabel();

 private:
  void updateText(const Uuid &selectedBank);
  void setFontColor(FrameBuffer &fb) const override;
};
