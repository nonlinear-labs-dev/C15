#pragma once

#include "Control.h"

class Label : public Control
{
  private:
    typedef Control super;

  public:
    Label (const Glib::ustring &text, const Rect &pos);
    Label (const Rect &pos);
    virtual ~Label ();

    bool redraw (FrameBuffer &fb) override;

    virtual bool setText (const Glib::ustring &text, int suffixLength = 0);
    virtual Glib::ustring getText () const;

    void setFontColor (FrameBuffer::Colors fontColor);

  protected:
    virtual Font::Justification getJustification () const;
    virtual int getFontHeight () const;
    virtual shared_ptr<Font> getFont () const;

    virtual Glib::ustring shortenStringIfNeccessary (shared_ptr<Font> font, const Glib::ustring &text) const;

    virtual void setFontColor (FrameBuffer &fb) const;
    virtual void setSuffixFontColor (FrameBuffer &fb) const;

    virtual int getXOffset () const;
    virtual int getYOffset () const;
    virtual int getRightMargin() const;

  private:
    Label (const Label& other);
    Label& operator= (const Label&);

    void drawSuffix (FrameBuffer &fb, int fullWidth, const Glib::ustring &firstPart, const Glib::ustring &secondPart);

    Glib::ustring m_text;
    int m_suffixLength = 0;
    FrameBuffer::Colors m_fontColor = FrameBuffer::Colors::Undefined;
};

