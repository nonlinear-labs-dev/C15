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
  BankNumberLabel(const Rect &pos);
  virtual ~BankNumberLabel();

 protected:
  void setBackgroundColor(FrameBuffer &fb) const override;
  void setFontColor(FrameBuffer &fb) const override;
  Font::Justification getJustification() const override;

 private:
  // private methods
  BankNumberLabel(const BankNumberLabel &other);
  BankNumberLabel &operator=(const BankNumberLabel &);

  void onBankSelectionChanged(const Uuid &selectedBank);
};
