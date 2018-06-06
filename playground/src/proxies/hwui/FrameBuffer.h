#pragma once

#include <cstdint>
#include "controls/Rect.h"
#include <playground.h>
#include <linux/fb.h>
#include <tools/Uncopyable.h>

class Font;
class UDPSender;

class FrameBuffer : public Uncopyable, public sigc::trackable
{
  public:
    virtual ~FrameBuffer ();

    static inline FrameBuffer &get ()
    {
      static FrameBuffer buf;
      return buf;
    }

    typedef int32_t tCoordinate;


    typedef uint8_t tPixel;

    enum Colors
      : tPixel
      {
        Undefined = 0xFF, C43 = 0x00, C77 = 0x02, C103 = 0x05, C128 = 0x06, C179 = 0x0A, C204 = 0x0B, C255 = 0x0F, Transparent = 0xFF
    };

    tPixel interpolateColor (float normalized)
    {
      return normalized * 0x0F;
    }


    void setColor (const Colors &c);
    void fiddleColor (tPixel p);

    Colors getColor () const;

    void fillRect (tCoordinate left, tCoordinate top, tCoordinate width, tCoordinate height);
    void fillRect (const Rect &rect);

    void drawRect (tCoordinate left, tCoordinate top, tCoordinate width, tCoordinate height);
    void drawRect (const Rect &rect);

    void drawHorizontalLine (tCoordinate x, tCoordinate y, tCoordinate length);
    void drawVerticalLine (tCoordinate x, tCoordinate y, tCoordinate length);

    void setPixel (tCoordinate x, tCoordinate y);
    void clear ();

    struct StackScopeGuard
    {
        FrameBuffer *m_fb;

        StackScopeGuard (FrameBuffer *fb);
        StackScopeGuard (StackScopeGuard &&other);
        virtual ~StackScopeGuard ();
        StackScopeGuard (const Uncopyable &) = delete;
        StackScopeGuard &operator= (const Uncopyable &) = delete;
    };

    struct Clip : StackScopeGuard
    {
        Clip (FrameBuffer *fb, const Rect &clip);
        Clip (Clip &&other);
        ~Clip ();

        bool isEmpty () const;
    };

    struct Offset : StackScopeGuard
    {
        Offset (FrameBuffer *fb, const Point &offset);
        Offset (Offset &&other);
        ~Offset ();
    };

    friend struct Clip;
    friend struct Offset;

    Clip clip (const Rect &rect);
    Clip clipRespectingOffset (const Rect &rect);
    Offset offset (const Point &offset);

    void swapBuffers();
    bool isValidColor(Colors c) const;

  private:
    FrameBuffer ();

    void setOffsetPixel (tCoordinate x, tCoordinate y);
    void setRawPixel (tCoordinate x, tCoordinate y);
    void drawRawHorizontalLine (tCoordinate x, tCoordinate y, tCoordinate length);

    void initStacks ();
    void openAndMap ();
    void unmapAndClose ();

    long getIndex(tCoordinate x, tCoordinate y) const;

    Colors m_currentColor = C43;
    vector<tPixel> m_backBuffer;
    std::stack<Rect> m_clips;
    std::stack<Point> m_offsets;
};

