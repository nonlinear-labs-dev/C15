#include <proxies/hwui/FrameBuffer.h>
#include <tools/ExceptionTools.h>
#include "Text.h"
#include "PrimitiveInstance.h"

namespace DescriptiveLayouts
{

  Text::Text(const PrimitiveInstance &e) :
      super(e.relativePosition),
      m_primitive(e)
  {
    if(!m_primitive.defaultText.empty()) {
      setText(m_primitive.defaultText, 0);
    }
  }

  Text::~Text()
  {
  }

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
    }
    throw ExceptionTools::TemplateException("unkown text align style for key: " + getStyleValue(StyleKey::TextAlign), "__LINE__ __FILE__");
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
        DisplayString a = std::any_cast < DisplayString > (value);
        setText(a.first, a.second);
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
}
