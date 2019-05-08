#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class SetupSelectionLabel : public SetupLabel
{
 public:
  SetupSelectionLabel(const Glib::ustring &text);
  virtual ~SetupSelectionLabel();

  bool redraw(FrameBuffer &fb) override;
  void drawBackground(FrameBuffer &fb) override;
};
