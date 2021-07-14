#pragma once

#include "PresetLabel.h"

class Uuid;

class NumPresetsInBankLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit NumPresetsInBankLabel(const Rect &pos);
  ~NumPresetsInBankLabel() override;

 private:
  void updateText(const Uuid &selectedBank);
  void setFontColor(FrameBuffer &fb) const override;
};
