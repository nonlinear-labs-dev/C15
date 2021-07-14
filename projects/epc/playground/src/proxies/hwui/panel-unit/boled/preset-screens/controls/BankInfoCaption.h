#pragma once

#include <proxies/hwui/controls/Label.h>
#include <sigc++/connection.h>

class FrameBuffer;
class Uuid;

class BankInfoCaption : public Label
{
 private:
  typedef Label super;

 public:
  explicit BankInfoCaption(const Rect &pos);
  ~BankInfoCaption() override;

  bool redraw(FrameBuffer &fb) override;

 private:
  void setFontColor(FrameBuffer &fb) const override;
  std::shared_ptr<Font> getFont() const override;
  int getFontHeight() const override;

  void onBankSelection(const Uuid &selectedBank);
  void onBankChanged();

  sigc::connection m_bankConnection;
};
