#include <proxies/hwui/FrameBuffer.h>
#include <tools/ExceptionTools.h>
#include "Text.h"
#include "proxies/hwui/descriptive-layouts/PrimitiveInstance.h"

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
    auto bg = (FrameBuffer::Colors) getStyleValue(StyleKey::BackgroundColor);

    if(bg != FrameBuffer::Colors::Transparent)
    {
      fb.setColor(bg);
      fb.fillRect(getPosition());
    }
  }

  void Text::setFontColor(FrameBuffer &fb) const
  {
    fb.setColor((FrameBuffer::Colors) getStyleValue(StyleKey::Color));
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

  void Text::setProperty(PrimitiveProperty key, std::any value)
  {
    switch(key)
    {
      case PrimitiveProperty::Text:
      {
        auto a = std::any_cast<DisplayString>(value);
        setText(StringAndSuffix(a.first, static_cast<size_t>(a.second)));
        break;
      }
      case PrimitiveProperty::Visibility:
      {
        bool vis = std::any_cast<bool>(value);
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

  std::shared_ptr<Font> Text::getFont() const
  {
    switch((StyleValues::Font) getStyleValue(StyleKey::FontDecoration))
    {
      case StyleValues::Font::Bold:
        return Oleds::get().getFont("Emphase_8_Bold", getFontHeight());
      case StyleValues::Font::Regular:
      default:
        return Oleds::get().getFont("Emphase_8_Regular", getFontHeight());
    }
  }
}
