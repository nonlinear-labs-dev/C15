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

  bool Circle::redraw(FrameBuffer &fb)
  {
    auto borderColor = (FrameBuffer::Colors) getStyleValue(StyleKey::BorderColor);
    fb.setColor(borderColor);
    fb.drawRect(getPosition());
    auto color = (FrameBuffer::Colors) getStyleValue(StyleKey::Color);
    fb.setColor(color);
    fb.fillCircle(getPosition().getPosition() + m_drawPosition, getHeight() / 2);
    return true;
  }

  void Circle::drawBackground(FrameBuffer &fb)
  {
    auto color = (FrameBuffer::Colors) getStyleValue(StyleKey::BackgroundColor);
    fb.setColor(color);
    fb.fillRect(getPosition());
  }

  void Circle::setDirty()
  {
    Control::setDirty();
  }

  void Circle::setProperty(PrimitiveProperty key, std::any value)
  {
    switch(key)
    {
      case PrimitiveProperty::Visibility:
        setVisible(std::any_cast<bool>(value));
        break;

      case PrimitiveProperty::ControlPosition:
        m_drawPosition = valueToPosition(std::any_cast<tControlPositionValue>(value));
        setDirty();
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
}
