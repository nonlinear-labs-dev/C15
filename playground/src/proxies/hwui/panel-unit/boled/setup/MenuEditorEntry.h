#pragma once

#include <proxies/hwui/panel-unit/boled/setup/SetupLabel.h>

class MenuEditorEntry : public SetupLabel
{
 private:
  typedef SetupLabel super;

 public:
  MenuEditorEntry();
  virtual ~MenuEditorEntry();

  void assign(const vector<Glib::ustring> &, int idx, bool selected);

 private:
  void setFontColor(FrameBuffer &fb) const override;
  void setBackgroundColor(FrameBuffer &fb) const override;
  Font::Justification getJustification() const override;
};
