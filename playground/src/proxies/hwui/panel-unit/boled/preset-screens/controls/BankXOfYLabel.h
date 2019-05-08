#pragma once

#include "proxies/hwui/controls/Label.h"

class Application;
class PresetBank;

class BankXOfYLabel : public Label
{
 private:
  typedef Label super;

 public:
  BankXOfYLabel(const Rect &pos);
  virtual ~BankXOfYLabel();

 protected:
  void setBackgroundColor(FrameBuffer &fb) const override;
  void setFontColor(FrameBuffer &fb) const override;
  Font::Justification getJustification() const override;

 private:
  void onBankSelectionChanged(shared_ptr<PresetBank> newBank);
};
