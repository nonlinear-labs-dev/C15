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
  explicit BankNameLabel(const Rect &pos);
  ~BankNameLabel() override;

  virtual void updateLabel(Bank *newBank);

 protected:
  void setBackgroundColor(FrameBuffer &fb) const override;
  void setFontColor(FrameBuffer &fb) const override;
  Font::Justification getJustification() const override;
  int getXOffset() const override;
  StringAndSuffix shortenStringIfNeccessary(const std::shared_ptr<Font> &font,
                                            const StringAndSuffix &text) const override;
};

class BankMidiLabel : public BankNameLabel
{
 private:
  typedef BankNameLabel super;

 public:
  explicit BankMidiLabel(const Rect &pos);
  ~BankMidiLabel() override;

  void updateLabel(Bank *newBank) override;

 protected:
  int getXOffset() const override;
  int getYOffset() const override;
};
