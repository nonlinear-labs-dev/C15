#pragma once

#include <cstdint>
#include "Rect.h"
#include "text2soled.h"
#include <linux/fb.h>

class Font;

class FrameBuffer
{
 public:
  virtual ~FrameBuffer();

  static inline FrameBuffer &get()
  {
    static FrameBuffer buf;
    return buf;
  }

  typedef int32_t tCoordinate;

  typedef uint8_t tPixel;

  enum Colors : tPixel
  {
    Undefined = 0xFF,
    C43 = 0x00,
    C77 = 0x02,
    C103 = 0x05,
    C128 = 0x06,
    C179 = 0x0A,
    C204 = 0x0B,
    C255 = 0x0F
  };

  tPixel interpolateColor(float normalized)
  {
    return normalized * 0x0F;
  }

  void setColor(const Colors &c);
  void fiddleColor(tPixel p);

  Colors getColor() const;

  void fillRect(tCoordinate left, tCoordinate top, tCoordinate width, tCoordinate height);
  void fillRect(const Rect &rect);

  void drawRect(tCoordinate left, tCoordinate top, tCoordinate width, tCoordinate height);
  void drawRect(const Rect &rect);

  void drawHorizontalLine(tCoordinate x, tCoordinate y, tCoordinate length);
  void drawVerticalLine(tCoordinate x, tCoordinate y, tCoordinate length);

  void setPixel(tCoordinate x, tCoordinate y);
  void clear();

 private:
  FrameBuffer();

  void setOffsetPixel(tCoordinate x, tCoordinate y);
  void setRawPixel(tCoordinate x, tCoordinate y);
  void drawRawHorizontalLine(tCoordinate x, tCoordinate y, tCoordinate length);

  void initStacks();
  void openAndMap();
  void unmapAndClose();

  long getIndex(tCoordinate x, tCoordinate y) const;

  Colors m_currentColor = C43;
  int m_fd = -1;
  size_t m_buffersize = 0;

  tPixel *m_frontBuffer = nullptr;
  struct fb_var_screeninfo m_varInfo;
};
