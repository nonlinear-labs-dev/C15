#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class PresetBank;
class Uuid;

class BankXOfYLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit BankXOfYLabel(const Rect &pos);
  ~BankXOfYLabel() override;

 protected:
  void setBackgroundColor(FrameBuffer &fb) const override;
  void setFontColor(FrameBuffer &fb) const override;
  Font::Justification getJustification() const override;

 private:
  void onBankSelectionChanged(const Uuid &selectedBank);
};
