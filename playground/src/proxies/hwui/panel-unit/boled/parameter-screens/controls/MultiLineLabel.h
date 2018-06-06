#pragma once

#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/Oleds.h>


class MultiLineLabel : public ControlWithChildren
{
  private:
    typedef ControlWithChildren super;

  public:
    MultiLineLabel (const Glib::ustring &text);

    bool setText (const Glib::ustring &text, FrameBuffer::Colors c);
    void setColor(FrameBuffer::Colors c);

    void drawBackground (FrameBuffer &fb) override;
    bool redraw (FrameBuffer &fb) override;
    virtual void setPosition (const Rect &rect) override;

  protected:
    virtual Oleds::tFont getFont ();

  private:
    void updateLines();

    FrameBuffer::Colors m_color = FrameBuffer::Colors::C103;
    Glib::ustring m_text;
};
