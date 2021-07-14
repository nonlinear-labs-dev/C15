#include <proxies/hwui/FrameBuffer.h>
#include <tools/ExceptionTools.h>
#include "Text.h"
#include "proxies/hwui/descriptive-layouts/PrimitiveInstance.h"
#include <proxies/hwui/Oleds.h>
#include <proxies/hwui/TextCropper.h>

namespace DescriptiveLayouts
{

  Text::Text(const PrimitiveInstance &e)
      : super(e.relativePosition)
      , m_primitive(e)
  {
  }

  Text::~Text() = default;

  void Text::drawBackground(FrameBuffer &fb)
  {
    if(isVisible())
    {
      auto bg = Control::isHighlight() ? (FrameBuffer::Colors) getStyleValue(StyleKey::HighlightBackgroundColor)
                                       : (FrameBuffer::Colors) getStyleValue(StyleKey::BackgroundColor);

      if(bg != FrameBufferColors::Transparent)
      {
        fb.setColor(bg);
        fb.fillRect(getPosition());
      }
    }
  }

  void Text::setFontColor(FrameBuffer &fb) const
  {
    auto color = Control::isHighlight() ? (FrameBuffer::Colors) getStyleValue(StyleKey::HighlightColor)
                                        : (FrameBuffer::Colors) getStyleValue(StyleKey::Color);
    fb.setColor(color);
  }

  void Text::setSuffixFontColor(FrameBuffer &fb) const
  {
    auto color = (FrameBuffer::Colors) getStyleValue(StyleKey::SuffixColor);
    fb.setColor(color);
  }

  Font::Justification Text::getJustification() const
  {
    switch((StyleValues::Alignment) getStyleValue(StyleKey::TextAlign))
    {
      case StyleValues::Alignment::Left:
        return Font::Justification::Left;

      case StyleValues::Alignment::Center:
        return Font::Justification::Center;

      case StyleValues::Alignment::Right:
        return Font::Justification::Right;

      default:
        throw ExceptionTools::TemplateException("unkown text align style for key: "
                                                    + std::to_string(getStyleValue(StyleKey::TextAlign)),
                                                "__LINE__ __FILE__");
    }
  }

  int Text::getFontHeight() const
  {
    return getStyleValue(StyleKey::FontSize);
  }

  StringAndSuffix Text::shortenStringIfNeccessary(const std::shared_ptr<Font> &font, const StringAndSuffix &text) const
  {
    return StringAndSuffix { TextCropper::shortenStringIfNeccessary(font, text.text, getPosition().getWidth()),
                             text.suffix };
  }

  void Text::setProperty(PrimitiveProperty key, std::experimental::any value)
  {
    switch(key)
    {
      case PrimitiveProperty::Text:
      {
        auto a = std::experimental::any_cast<DisplayString>(value);
        setText(StringAndSuffix(a.first, static_cast<size_t>(a.second)));
        break;
      }
      case PrimitiveProperty::Highlight:
      {
        Control::setHighlight(std::experimental::any_cast<bool>(value));
        break;
      }
      case PrimitiveProperty::Visibility:
      {
        bool vis = std::experimental::any_cast<bool>(value);
        setVisible(vis);
        break;
      }
      default:
        DebugLevel::throwException("Property", toString(key), "not implemented for Text");
    }
  }

  void Text::setDirty()
  {
    Control::setDirty();
  }

  const PrimitiveInstance &Text::getPrimitive() const
  {
    return m_primitive;
  }

  void Text::applyStyle(const StyleMap &style)
  {
    m_cachedFont = nullptr;
    Styleable::applyStyle(style);
  }

  std::shared_ptr<Font> Text::getFont() const
  {

    if(!m_cachedFont)
    {
      switch((StyleValues::Font) getStyleValue(StyleKey::FontDecoration))
      {
        case StyleValues::Font::Bold:
          m_cachedFont = Oleds::get().getFont("Emphase-8-Bold", getFontHeight());
          break;
        case StyleValues::Font::Regular:
        default:
          m_cachedFont = Oleds::get().getFont("Emphase-8-Regular", getFontHeight());
          break;
      }
    }

    return m_cachedFont;
  }
}
