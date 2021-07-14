#pragma once

#include "Control.h"
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/Font.h>
#include <memory>
#include <tools/StringAndSuffix.h>

class Label : public Control
{
 private:
  typedef Control super;

 public:
  Label(StringAndSuffix text, const Rect &pos);
  explicit Label(const Rect &pos);
  ~Label() override;

  bool redraw(FrameBuffer &fb) override;

  virtual bool setText(const StringAndSuffix &text);
  virtual StringAndSuffix getText() const;

  virtual std::shared_ptr<Font> getFont() const;
  void setFontColor(FrameBufferColors fontColor);
  FrameBufferColors getFontColor() const;
  void setJustification(const Font::Justification &justification);

 protected:
  virtual Font::Justification getJustification() const;
  virtual int getFontHeight() const;

  virtual StringAndSuffix shortenStringIfNeccessary(const std::shared_ptr<Font> &font,
                                                    const StringAndSuffix &text) const;

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
  FrameBufferColors m_fontColor = FrameBufferColors::Undefined;
  Font::Justification m_justification = Font::Justification::Center;
};
