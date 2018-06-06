#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupEditor.h>
#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class Setting;

class SignalFlowIndicatorEditor: public SetupLabel, public SetupEditor {
private:
  typedef SetupLabel super;

public:
  SignalFlowIndicatorEditor ();
  virtual ~SignalFlowIndicatorEditor ();

  virtual Font::Justification getJustification () const override;

private:
  bool redraw (FrameBuffer &fb) override;
  void setBackgroundColor (FrameBuffer &fb) const override;
  void onSettingChanged (const Setting *s);
  bool onButton (Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary (int inc, ButtonModifiers modifiers) override;
  void setFontColor (FrameBuffer &fb) const override;

  SetupLabel *m_label = nullptr;
};

