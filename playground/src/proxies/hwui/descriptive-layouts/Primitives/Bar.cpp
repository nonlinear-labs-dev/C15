#include "Bar.h"
#include "proxies/hwui/descriptive-layouts/PrimitiveInstance.h"

namespace DescriptiveLayouts
{

  Bar::Bar(const PrimitiveInstance &e)
      : Control(e.relativePosition)
      , m_primitive(e)
      , m_range(0.0, 1.0)
  {
  }

  Bar::~Bar()
  {
  }

  bool Bar::redraw(FrameBuffer &fb)
  {
    auto color = (FrameBuffer::Colors) getStyleValue(StyleKey::Color);

    Rect r = getPosition();

    double controlWidth = r.getWidth();
    double controlLeft = r.getLeft();

    auto from = std::min(m_range.first, m_range.second);
    auto to = std::max(m_range.first, m_range.second);

    auto left = round(r.getLeft() + from * r.getWidth());
    auto right = round(r.getLeft() + to * r.getWidth());
    r.setLeft(left);
    r.setWidth(right - left);
    r.normalize();

    fb.setColor(color);
    fb.fillRect(r);

    return true;
  }

  void Bar::drawBackground(FrameBuffer &fb)
  {
  }

  void Bar::setDirty()
  {
    Control::setDirty();
  }

  void Bar::setProperty(PrimitiveProperty key, std::any value)
  {
    switch(key)
    {
      case PrimitiveProperty::Range:
        if(std::exchange(m_range, std::any_cast<Range>(value)) != m_range)
          setDirty();
        break;

      case PrimitiveProperty::Visibility:
        setVisible(std::any_cast<bool>(value));
        break;
    }
  }

  const PrimitiveInstance &Bar::getPrimitive() const
  {
    return m_primitive;
  }
}
