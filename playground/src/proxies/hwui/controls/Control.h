#pragma once

#include "playground.h"
#include "Rect.h"
#include <nltools/Uncopyable.h>
#include <proxies/hwui/Font.h>
#include <proxies/hwui/FrameBuffer.h>

class Font;
class FrameBuffer;

class Control : public sigc::trackable, public Uncopyable
{
 public:
  Control(const Rect &pos);
  virtual ~Control();

  virtual bool redraw(FrameBuffer &fb);
  virtual void drawBackground(FrameBuffer &fb);

  virtual const Rect &getPosition() const;
  virtual bool isDirty() const;
  void setClean();

  virtual void setHighlight(bool isHighlight);
  bool isHighlight() const;

  virtual void setDirty();
  virtual bool isVisible() const;
  void setVisible(bool b);
  int getHeight() const;
  int getWidth() const;
  virtual void setPosition(const Rect &rect);

 protected:
  virtual void setBackgroundColor(FrameBuffer &fb) const;

 private:
  Rect m_rect;
  bool m_isDirty;
  bool m_highlight;
  bool m_visible;
};
