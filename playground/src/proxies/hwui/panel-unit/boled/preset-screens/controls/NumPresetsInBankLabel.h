#pragma once

#include "PresetLabel.h"

class NumPresetsInBankLabel : public Label
{
 private:
  typedef Label super;

 public:
  NumPresetsInBankLabel(const Rect &pos);
  virtual ~NumPresetsInBankLabel();

 private:
  void updateText(shared_ptr<PresetBank> bank);
  void setFontColor(FrameBuffer &fb) const override;
};
