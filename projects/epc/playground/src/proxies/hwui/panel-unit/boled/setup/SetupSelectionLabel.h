#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class SetupSelectionLabel : public SetupLabel
{
 public:
  explicit SetupSelectionLabel(const Glib::ustring &text);
  ~SetupSelectionLabel() override;

  bool redraw(FrameBuffer &fb) override;
  void drawBackground(FrameBuffer &fb) override;
};
