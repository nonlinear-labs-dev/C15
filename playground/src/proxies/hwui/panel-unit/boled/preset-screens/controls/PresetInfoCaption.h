#pragma once

#include <proxies/hwui/controls/Label.h>
#include <presets/PresetManager.h>

class FrameBuffer;
class Application;
class Parameter;
class PresetBank;

class PresetInfoCaption : public Label
{
 private:
  typedef Label super;

 public:
  PresetInfoCaption(const Rect &pos);
  virtual ~PresetInfoCaption();

  bool redraw(FrameBuffer &fb) override;

 private:
  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

  void onBankSelection(PresetManager::tBankPtr bank);
  void onBankChanged();

  sigc::connection m_bankConnection;
};
