#pragma once

#include "Control.h"

class Label : public Control
{
 private:
  typedef Control super;

 public:
  struct StringAndSuffix
  {
    StringAndSuffix(const Glib::ustring &first, const Glib::ustring &second)
        : text(first + second)
        , suffix(second.length())
    {
    }

    StringAndSuffix(const Glib::ustring &first, size_t suffix = 0)
        : text(first)
        , suffix(suffix)
    {
    }

    StringAndSuffix(const char *first, size_t suffix = 0)
        : text(first)
        , suffix(suffix)
    {
    }

    bool operator!=(const StringAndSuffix &other) const
    {

      return text.raw() != other.text.raw() || suffix != other.suffix;
    }

    std::pair<Glib::ustring, Glib::ustring> getSplits() const
    {
      return std::make_pair<Glib::ustring, Glib::ustring>(text.substr(0, text.length() - suffix),
                                                          text.substr(text.length() - suffix));
    }

    Glib::ustring text;
    size_t suffix = 0;
  };

  Label(const StringAndSuffix &text, const Rect &pos);
  Label(const Rect &pos);
  ~Label() override;

  bool redraw(FrameBuffer &fb) override;

  virtual bool setText(const StringAndSuffix &text);
  virtual StringAndSuffix getText() const;

  virtual std::shared_ptr<Font> getFont() const;
  void setFontColor(FrameBuffer::Colors fontColor);

 protected:
  virtual Font::Justification getJustification() const;
  virtual int getFontHeight() const;

  virtual StringAndSuffix shortenStringIfNeccessary(std::shared_ptr<Font> font, const StringAndSuffix &text) const;

  virtual void setFontColor(FrameBuffer &fb) const;
  virtual void setSuffixFontColor(FrameBuffer &fb) const;
  virtual int getXOffset() const;
  virtual int getYOffset() const;
  virtual int getRightMargin() const;

 private:
  virtual void drawSuffix(FrameBuffer &fb, int fullWidth, const Glib::ustring &firstPart,
                          const Glib::ustring &secondPart);

  Label(const Label &other);
  Label &operator=(const Label &);

  StringAndSuffix m_text;
  FrameBuffer::Colors m_fontColor = FrameBuffer::Colors::Undefined;
};
