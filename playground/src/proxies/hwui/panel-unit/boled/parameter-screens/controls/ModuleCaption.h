#pragma once

#include "proxies/hwui/controls/Label.h"
#include <Application.h>
#include <presets/PresetManager.h>
#include <presets/EditBuffer.h>
#include <groups/MasterGroup.h>
#include <groups/ScaleGroup.h>
#include <groups/UnisonGroup.h>

class Application;
class Parameter;

class ModuleCaption : public Label
{
 private:
  typedef Label super;

 public:
  ModuleCaption(const Rect &pos);

  bool redraw(FrameBuffer &fb) override;

 private:
  virtual void setFontColor(FrameBuffer &fb) const override;
  virtual std::shared_ptr<Font> getFont() const override;
  virtual int getFontHeight() const override;

 protected:
  virtual void updateText(Parameter *newOne);

  StringAndSuffix shortenStringIfNeccessary(std::shared_ptr<Font> font, const StringAndSuffix &text) const override;

private:
  void onParameterSelected(Parameter *newOne);
  void onSelectionChanged(VoiceGroup v);
  void onSoundTypeChanged();

  bool enableVoiceGroupSuffix() const;
};