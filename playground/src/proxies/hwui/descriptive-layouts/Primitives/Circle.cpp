#include <proxies/hwui/descriptive-layouts/TemplateEnums.h>
#include "Circle.h"

namespace DescriptiveLayouts
{

  Circle::Circle(const PrimitiveInstance &e)
      : Control(e.relativePosition)
      , m_range(0.0, 1.0)
      , m_primitive(e)
      , m_steps(5)
      , m_drawPosition(getPosition().getLeft(), getPosition().getTop() + getPosition().getHeight() / 2)
  {
  }

  Circle::~Circle()
  {
  }

  bool Circle::redraw(FrameBuffer &fb)
  {
    auto color = (FrameBuffer::Colors) getStyleValue(StyleKey::Color);
    fb.setColor(color);
    fb.fillCircle(m_drawPosition, m_primitive.relativePosition.getHeight() / 2, m_steps);
    return true;
  }

  void Circle::drawBackground(FrameBuffer &fb)
  {
      auto color =(FrameBuffer::Colors)getStyleValue(StyleKey::BackgroundColor);
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

  const Point Circle::valueToPosition(tControlPositionValue range) const
  {
    Point p(getPosition().getLeft(), getPosition().getCenter().getY());
    const auto controlPos = range;
    const auto totalWidth = getPosition().getWidth();
    p.moveBy(totalWidth * controlPos, 0);
    return p;
  }
}
