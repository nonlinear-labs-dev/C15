#include "MultiLineLabel.h"
#include <proxies/hwui/Oleds.h>
#include <proxies/hwui/controls/Label.h>
#include <tools/TextSplitter.h>
#include <proxies/hwui/controls/LeftAlignedLabel.h>
#include <proxies/hwui/FrameBuffer.h>

namespace DETAIL
{
  class ChildLabel : public LeftAlignedLabel
  {
   public:
    ChildLabel(const Glib::ustring &line, FrameBufferColors color, Oleds::tFont font, const Rect &pos)
        : LeftAlignedLabel(line, pos)
        , m_font(font)
    {
      setFontColor(color);
    }

    virtual std::shared_ptr<Font> getFont() const override
    {
      return m_font;
    }

   private:
    Oleds::tFont m_font;
  };
}

MultiLineLabel::MultiLineLabel(Glib::ustring text)
    : super(Rect(0, 0, 0, 0))
    , m_text(std::move(text))
{
}

MultiLineLabel::MultiLineLabel(const Rect &r, Glib::ustring text)
    : ControlWithChildren(r)
    , m_text(std::move(text))
{
  setText(m_text, FrameBufferColors::C255);
}

void MultiLineLabel::setColor(FrameBufferColors c)
{
  m_color = c;
}

bool MultiLineLabel::setText(const Glib::ustring &text, FrameBufferColors c)
{
  if(m_text != text || m_color != c)
  {
    m_text = text;
    m_color = c;
    updateLines();
    return true;
  }
  return false;
}

void MultiLineLabel::setPosition(const Rect &rect)
{
  bool widthChanged = getPosition().getWidth() != rect.getWidth();

  super::setPosition(rect);

  if(widthChanged || m_forceUpdateOnSetPos)
  {
    updateLines();
  }
}

std::shared_ptr<Font> MultiLineLabel::getFont()
{
  return Oleds::get().getFont("Emphase-8-Regular", 8);
}

void MultiLineLabel::updateLines()
{
  Rect r = getPosition();

  clear();

  auto font = getFont();

  TextSplitter s(font, r.getWidth(), m_text);
  int lineHeight = font->getHeight() + 3;
  int y = 0;

  for(const auto &line : s.getLines())
  {
    addControl(new DETAIL::ChildLabel(line, m_color, font, Rect(0, y, r.getWidth(), lineHeight)));
    y += lineHeight;
  }

  super::setPosition(Rect(r.getLeft(), r.getTop(), r.getWidth(), y));
}

void MultiLineLabel::drawBackground(FrameBuffer &fb)
{
  const Rect &r = getPosition();
  if(isVisible())
    setBackgroundColor(fb);
  else
    fb.setColor(FrameBufferColors::Transparent);
  fb.fillRect(r);
}

bool MultiLineLabel::redraw(FrameBuffer &fb)
{
  return super::redraw(fb);
}

const Glib::ustring &MultiLineLabel::getText() const
{
  return m_text;
}
