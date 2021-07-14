#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class PresetBank;
class Uuid;

class BankNumberLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit BankNumberLabel(const Rect &pos);
  ~BankNumberLabel() override;

 protected:
  void setBackgroundColor(FrameBuffer &fb) const override;
  void setFontColor(FrameBuffer &fb) const override;
  Font::Justification getJustification() const override;

 private:
  BankNumberLabel(const BankNumberLabel &other);
  BankNumberLabel &operator=(const BankNumberLabel &);

  void onBankSelectionChanged(const Uuid &selectedBank);
};
