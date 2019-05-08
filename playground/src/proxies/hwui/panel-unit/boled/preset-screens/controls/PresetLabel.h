#pragma once

#include "proxies/hwui/controls/Label.h"
#include <tools/Throttler.h>

class Application;
class PresetBank;
class Preset;

class PresetLabel : public Label
{
 private:
  typedef Label super;

 public:
  PresetLabel(const Rect &pos);
  virtual ~PresetLabel();

  void setText(const Glib::ustring &text, bool selected, bool loaded);

 protected:
  void setFontColor(FrameBuffer &fb) const;
  bool showsLoadedPreset() const;
  bool showsSelectedPreset() const;

 private:
  bool m_selected = false;
  bool m_loaded = false;
};
