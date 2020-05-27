#pragma once

#include <cstdint>
#include "controls/Rect.h"
#include <playground.h>
#include <linux/fb.h>
#include <nltools/Uncopyable.h>
#include <stack>
#include <vector>
#include <sigc++/trackable.h>
#include <proxies/hwui/HWUIEnums.h>

class Font;
class UDPSender;

class FrameBuffer : public Uncopyable, public sigc::trackable
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
  using Colors = ::FrameBufferColors;

  tPixel interpolateColor(float normalized)
  {
    return normalized * 0x0F;
  }

  void setColor(const Colors &c);
  void fiddleColor(tPixel p);

  Colors getColor() const;

  void fillRect(tCoordinate left, tCoordinate top, tCoordinate width, tCoordinate height);
  void fillRect(const Rect &rect);
  void fillCircle(const Point &center, int radius);

  void drawRect(tCoordinate left, tCoordinate top, tCoordinate width, tCoordinate height);
  void drawRect(const Rect &rect);

  void drawHorizontalLine(tCoordinate x, tCoordinate y, tCoordinate length);
  void drawVerticalLine(tCoordinate x, tCoordinate y, tCoordinate length);

  void setPixel(tCoordinate x, tCoordinate y);
  void clear();

  void setPerPixelDebug(bool onOff);

  struct StackScopeGuard
  {
    FrameBuffer *m_fb;

    StackScopeGuard(FrameBuffer *fb);
    StackScopeGuard(StackScopeGuard &&other);
    virtual ~StackScopeGuard();
    StackScopeGuard(const Uncopyable &) = delete;
    StackScopeGuard &operator=(const Uncopyable &) = delete;
  };

  struct Clip : StackScopeGuard
  {
    Clip(FrameBuffer *fb, const Rect &clip);
    Clip(Clip &&other);
    ~Clip();

    bool isEmpty() const;
  };

  struct Offset : StackScopeGuard
  {
    Offset(FrameBuffer *fb, const Point &offset);
    Offset(Offset &&other);
    ~Offset();
  };

  friend struct Clip;
  friend struct Offset;

  Clip clip(const Rect &rect);
  Clip clipRespectingOffset(const Rect &rect);
  Offset offset(const Point &offset);

  bool swapBuffers();
  bool isValidColor(Colors c) const;

 private:
  FrameBuffer();

  void setOffsetPixel(tCoordinate x, tCoordinate y);
  void setRawPixel(tCoordinate x, tCoordinate y);
  void drawRawHorizontalLine(tCoordinate x, tCoordinate y, tCoordinate length);

  void initStacks();
  void openAndMap();
  void unmapAndClose();

  long getIndex(tCoordinate x, tCoordinate y) const;

  Colors m_currentColor = Colors::C43;
  std::vector<tPixel> m_backBuffer;
  std::stack<Rect> m_clips;
  std::stack<Point> m_offsets;
  bool m_perPixelDebug = false;
};
