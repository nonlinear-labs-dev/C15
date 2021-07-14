#pragma once

#include <proxies/hwui/controls/Label.h>

class Uuid;

class PresetInfoCaption : public Label
{
 private:
  typedef Label super;

 public:
  explicit PresetInfoCaption(const Rect &pos);
  ~PresetInfoCaption() override;

  bool redraw(FrameBuffer &fb) override;

 private:
  void setFontColor(FrameBuffer &fb) const override;
  std::shared_ptr<Font> getFont() const override;
  int getFontHeight() const override;

  void onBankSelection(const Uuid &selectedBank);
  void onBankChanged();

  sigc::connection m_bankConnection;
};
