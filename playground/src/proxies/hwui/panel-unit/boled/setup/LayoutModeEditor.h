#pragma once

#include "SetupLabel.h"
#include "SetupEditor.h"
class LayoutModeEditor : public SetupLabel, public SetupEditor
{
 public:
  typedef SetupLabel super;
  LayoutModeEditor();
  ~LayoutModeEditor() override;
  Font::Justification getJustification() const override;

 private:
  bool redraw(FrameBuffer &fb) override;
  void setBackgroundColor(FrameBuffer &fb) const override;
  void onSettingChanged(const Setting *s);
  bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
  bool onRotary(int inc, ButtonModifiers modifiers) override;
  void setFontColor(FrameBuffer &fb) const override;

  SetupLabel *m_label = nullptr;
};
