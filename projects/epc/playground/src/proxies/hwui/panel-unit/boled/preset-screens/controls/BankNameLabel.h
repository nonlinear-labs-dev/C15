#pragma once

#include <tools/Uuid.h>
#include "proxies/hwui/controls/Label.h"

class Application;
class Bank;

class BankNameLabel : public Label
{
 private:
  typedef Label super;

 public:
  BankNameLabel(const Rect &pos);
  virtual ~BankNameLabel();

  void updateLabel(Bank *newBank);
  StringAndSuffix getText() const override;

 protected:
  void setBackgroundColor(FrameBuffer &fb) const override;
  void setFontColor(FrameBuffer &fb) const override;
  Font::Justification getJustification() const override;
  int getXOffset() const override;
  virtual StringAndSuffix shortenStringIfNeccessary(std::shared_ptr<Font> font,
                                                    const StringAndSuffix &text) const override;
  void onMidiSelectionHappened(const Uuid &uuid);

  Bank *lastBank = nullptr;
  bool isMidiBank() const;
  StringAndSuffix addMidiSuffix(StringAndSuffix suffix) const;
};
