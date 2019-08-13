#include <tools/ExceptionTools.h>
#include "Border.h"
#include "proxies/hwui/descriptive-layouts/PrimitiveInstance.h"

namespace DescriptiveLayouts
{

  Border::Border(const PrimitiveInstance &e)
      : Control(e.relativePosition)
      , m_primitive(e)
  {
  }

  Border::~Border() = default;

  void Border::drawBackground(FrameBuffer &fb)
  {
  }

  bool Border::redraw(FrameBuffer &fb)
  {
    auto style = (StyleValues::BorderStyle) getStyleValue(StyleKey::BorderStyle);
    auto color = (FrameBuffer::Colors) getStyleValue(StyleKey::Color);

    if(Control::isHighlight())
      color = (FrameBuffer::Colors) getStyleValue(StyleKey::HighlightColor);

    switch(style)
    {
      case StyleValues::BorderStyle::None:
        return false;

      case StyleValues::BorderStyle::Rounded:
        fb.setColor(color);
        getPosition().drawRounded(fb);
        return true;

      case StyleValues::BorderStyle::Solid:
        fb.setColor(color);
        fb.drawRect(getPosition());
        return true;
    }
    throw ExceptionTools::TemplateException(
        "unkown border style: " + std::to_string(getStyleValue(StyleKey::BorderStyle)), "__LINE__ __FILE__");
    return true;
  }

  void Border::setDirty()
  {
    Control::setDirty();
  }

  const PrimitiveInstance &Border::getPrimitive() const
  {
    return m_primitive;
  }

  void Border::setProperty(PrimitiveProperty key, std::experimental::any value)
  {
    switch(key)
    {
      case PrimitiveProperty::Highlight:
      {
        Control::setHighlight(std::experimental::any_cast<bool>(value));
        break;
      }
      case PrimitiveProperty::Visibility:
      {
        auto boolean = std::experimental::any_cast<bool>(value);
        setVisible(boolean);
      }
      break;
      case PrimitiveProperty::Range:
      case PrimitiveProperty::ControlPosition:
      case PrimitiveProperty::None:
      case PrimitiveProperty::Text:
        break;
    }
  }
}
