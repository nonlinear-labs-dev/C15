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

  Border::~Border()
  {
  }

  void Border::drawBackground(FrameBuffer &fb)
  {
  }

  bool Border::redraw(FrameBuffer &fb)
  {
    auto style = (StyleValues::BorderStyle) getStyleValue(StyleKey::BorderStyle);
    auto color = (FrameBuffer::Colors) getStyleValue(StyleKey::Color);

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

  void Border::setProperty(PrimitiveProperty key, std::any value)
  {
    switch(key)
    {
      case PrimitiveProperty::Visibility:
      {
        auto boolean = std::any_cast<bool>(value);
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
