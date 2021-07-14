#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/HWUIEnums.h>

class MultiLineLabel : public ControlWithChildren
{
 private:
  typedef ControlWithChildren super;

 public:
  explicit MultiLineLabel(Glib::ustring text);
  MultiLineLabel(const Rect &r, Glib::ustring text);

  bool setText(const Glib::ustring &text, FrameBufferColors c);
  const Glib::ustring &getText() const;
  virtual void setColor(FrameBufferColors c);

  void drawBackground(FrameBuffer &fb) override;
  bool redraw(FrameBuffer &fb) override;
  void setPosition(const Rect &rect) override;

 protected:
  virtual std::shared_ptr<Font> getFont();

 private:
  bool m_forceUpdateOnSetPos = false;
  void updateLines();

  FrameBufferColors m_color = FrameBufferColors::C103;
  Glib::ustring m_text;
};
