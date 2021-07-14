#pragma once

#include "SetupLabel.h"
#include "SetupEditor.h"
#include <Application.h>
#include <device-settings/Settings.h>
#include <proxies/hwui/FrameBuffer.h>

template <typename tSetting> class NumericSettingEditor : public SetupLabel, public SetupEditor
{
 public:
  NumericSettingEditor()
      : SetupLabel({ 0, 0, 0, 0 })
  {
    auto setting = Application::get().getSettings()->getSetting<tSetting>();
    setting->onChange(mem_fun(this, &NumericSettingEditor<tSetting>::onSettingChanged));
  }

  bool onRotary(int inc, ButtonModifiers modifiers) override
  {
    Application::get().getSettings()->getSetting<tSetting>()->incDec(inc, modifiers);
    return true;
  }

  void onSettingChanged(const Setting *s)
  {
    auto setting = dynamic_cast<const tSetting *>(s);
    setText({ setting->getDisplayString(), 0 });
  }

 private:
  void setBackgroundColor(FrameBuffer &fb) const override
  {
    fb.setColor(FrameBufferColors::C103);
  }

  Font::Justification getJustification() const override
  {
    return Font::Justification::Left;
  }

  bool redraw(FrameBuffer &fb) override
  {
    SetupLabel::redraw(fb);

    fb.setColor(FrameBufferColors::C179);
    fb.drawRect(getPosition());
    return true;
  }

  void setFontColor(FrameBuffer &fb) const override
  {
    fb.setColor(FrameBufferColors::C255);
  }
};

template <typename tSetting> class BooleanSettingEditor : public SetupLabel, public SetupEditor
{
 public:
  BooleanSettingEditor()
      : SetupLabel({ 0, 0, 0, 0 })
  {
    auto setting = Application::get().getSettings()->getSetting<tSetting>();
    setting->onChange(mem_fun(this, &BooleanSettingEditor<tSetting>::onSettingChanged));
  }

  bool onRotary(int inc, ButtonModifiers modifiers) override
  {
    Application::get().getSettings()->getSetting<tSetting>()->toggle();
    return true;
  }

  void onSettingChanged(const Setting *s)
  {
    auto setting = dynamic_cast<const tSetting *>(s);
    setText({ setting->getDisplayString(), 0 });
  }

 private:
  void setBackgroundColor(FrameBuffer &fb) const override
  {
    fb.setColor(FrameBufferColors::C103);
  }

  Font::Justification getJustification() const override
  {
    return Font::Justification::Left;
  }

  bool redraw(FrameBuffer &fb) override
  {
    SetupLabel::redraw(fb);

    fb.setColor(FrameBufferColors::C179);
    fb.drawRect(getPosition());
    return true;
  }

  void setFontColor(FrameBuffer &fb) const override
  {
    fb.setColor(FrameBufferColors::C255);
  }
};