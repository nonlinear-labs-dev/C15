#pragma once

#include <proxies/hwui/controls/Label.h>
#include <presets/PresetManager.h>

class FrameBuffer;
class Uuid;

class BankInfoCaption : public Label
{
 private:
  typedef Label super;

 public:
  BankInfoCaption(const Rect &pos);
  virtual ~BankInfoCaption();

  bool redraw(FrameBuffer &fb) override;

 private:
  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual std::shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

  void onBankSelection(const Uuid &selectedBank);
  void onBankChanged();

  sigc::connection m_bankConnection;
};
