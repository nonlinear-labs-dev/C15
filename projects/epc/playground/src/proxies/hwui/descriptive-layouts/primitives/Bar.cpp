#include "Bar.h"
#include "proxies/hwui/descriptive-layouts/PrimitiveInstance.h"
#include <proxies/hwui/FrameBuffer.h>
#include <math.h>

namespace DescriptiveLayouts
{

  Bar::Bar(const PrimitiveInstance &e)
      : Control(e.relativePosition)
      , m_primitive(e)
      , m_range(0.0, 1.0)
  {
  }

  Bar::~Bar() = default;

  bool Bar::redraw(FrameBuffer &fb)
  {
    if(isVisible())
    {
      auto color = Control::isHighlight() ? (FrameBuffer::Colors) getStyleValue(StyleKey::HighlightColor)
                                          : (FrameBuffer::Colors) getStyleValue(StyleKey::Color);
      Rect r = getPosition();

      auto from = std::min(m_range.first, m_range.second);
      auto to = std::max(m_range.first, m_range.second);

      auto left = round(r.getLeft() + from * r.getWidth());
      auto right = round(r.getLeft() + to * r.getWidth());
      r.setLeft(static_cast<int>(left));
      r.setWidth(static_cast<int>(right - left));
      r.normalize();

      fb.setColor(color);
      fb.fillRect(r);
    }

    return true;
  }

  void Bar::drawBackground(FrameBuffer &fb)
  {
    if(Control::isHighlight())
    {
      fb.setColor((FrameBuffer::Colors) getStyleValue(StyleKey::HighlightBackgroundColor));
    }
    else
    {
      fb.setColor((FrameBuffer::Colors) getStyleValue(StyleKey::BackgroundColor));
    }

    if(isVisible())
    {
      fb.fillRect(getPosition());
    }
  }

  void Bar::setDirty()
  {
    Control::setDirty();
  }

  void Bar::setProperty(PrimitiveProperty key, std::experimental::any value)
  {
    switch(key)
    {
      case PrimitiveProperty::Range:
        if(std::exchange(m_range, std::experimental::any_cast<Range>(value)) != m_range)
          setDirty();
        break;
      case PrimitiveProperty::Highlight:
        Control::setHighlight(std::experimental::any_cast<bool>(value));
        break;
      case PrimitiveProperty::Visibility:
        setVisible(std::experimental::any_cast<bool>(value));
        break;
    }
  }

  const PrimitiveInstance &Bar::getPrimitive() const
  {
    return m_primitive;
  }

  void Bar::applyStyle(const StyleMap &style)
  {
    Styleable::applyStyle(style);
  }
}
