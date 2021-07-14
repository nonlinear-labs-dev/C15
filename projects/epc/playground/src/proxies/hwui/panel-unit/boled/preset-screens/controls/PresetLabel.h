#pragma once

#include "proxies/hwui/controls/Label.h"

class PresetLabel : public Label
{
 private:
  typedef Label super;

 public:
  explicit PresetLabel(const Rect &pos);
  ~PresetLabel() override;

  void setText(const Glib::ustring &text, bool selected, bool loaded);

 protected:
  void setFontColor(FrameBuffer &fb) const override;
  bool showsLoadedPreset() const;
  bool showsSelectedPreset() const;

 private:
  bool m_selected = false;
  bool m_loaded = false;
};
