#include <proxies/hwui/descriptive-layouts/TemplateEnums.h>
#include "Circle.h"

namespace DescriptiveLayouts
{

  Circle::Circle(const PrimitiveInstance &e)
      : Control(e.relativePosition)
      , m_range(0.0, 1.0)
      , m_valueDimension(e.relativePosition.getX() + e.relativePosition.getHeight() / 2, e.relativePosition.getY(),
                         e.relativePosition.getWidth() - e.relativePosition.getHeight(), e.relativePosition.getHeight())
      , m_primitive(e)
      , m_drawPosition(getPosition().getLeftTop())
  {
  }

  Circle::~Circle() = default;

  void Circle::redrawBorder(FrameBuffer &fb)
  {
    auto style = (StyleValues::BorderStyle) getStyleValue(StyleKey::BorderStyle);
    auto color = (FrameBuffer::Colors) getStyleValue(StyleKey::BorderColor);

    switch(style)
    {
      case StyleValues::BorderStyle::None:
        break;
      case StyleValues::BorderStyle::Rounded:
        fb.setColor(color);
        getPosition().drawRounded(fb);
        break;
      case StyleValues::BorderStyle::Solid:
        fb.setColor(color);
        getPosition().draw(fb);
        break;
    }
  }

  bool Circle::redraw(FrameBuffer &fb)
  {
    redrawBorder(fb);

    auto color = Control::isHighlight() ? (FrameBuffer::Colors) getStyleValue(StyleKey::HighlightColor)
                                        : (FrameBuffer::Colors) getStyleValue(StyleKey::Color);
    fb.setColor(color);
    fb.fillCircle(getPosition().getPosition() + m_drawPosition, getHeight() / 2);
    return true;
  }

  void Circle::drawBackground(FrameBuffer &fb)
  {
    auto color = Control::isHighlight() ? (FrameBuffer::Colors) getStyleValue(StyleKey::BackgroundColor)
                                        : (FrameBuffer::Colors) getStyleValue(StyleKey::HighlightBackgroundColor);
    fb.setColor(color);
    fb.fillRect(getPosition());
  }

  void Circle::setProperty(PrimitiveProperty key, std::experimental::any value)
  {
    switch(key)
    {
      case PrimitiveProperty::Visibility:
        setVisible(std::experimental::any_cast<bool>(value));
        break;
      case PrimitiveProperty::Highlight:
        Control::setHighlight(std::experimental::any_cast<bool>(value));
        break;
      case PrimitiveProperty::ControlPosition:
        m_drawPosition = valueToPosition(std::experimental::any_cast<tControlPositionValue>(value));
        Control::setDirty();
        break;
      case PrimitiveProperty::None:
      case PrimitiveProperty::Text:
      case PrimitiveProperty::Range:
        break;
    }
  }

  const PrimitiveInstance &Circle::getPrimitive() const
  {
    return m_primitive;
  }

  const Point Circle::valueToPosition(tControlPositionValue controlPos) const
  {
    Point p(getPosition().getLeft(), getPosition().getTop());
    p.moveBy(static_cast<int>(m_valueDimension.getWidth() * controlPos), 0);
    return p;
  }

  void Circle::setDirty()
  {
    Control::setDirty();
  }
}
