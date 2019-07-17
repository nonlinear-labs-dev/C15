#pragma once

#include <proxies/hwui/controls/Control.h>
#include <proxies/hwui/descriptive-layouts/Styleable.h>
#include <proxies/hwui/descriptive-layouts/PropertyOwner.h>
#include <any>

namespace DescriptiveLayouts
{
  class PrimitiveInstance;

  class Circle : public Control, public Styleable, public PropertyOwner
  {
   public:
    Circle(const PrimitiveInstance &e);
    virtual ~Circle();

    using Range = std::pair<tControlPositionValue, tControlPositionValue>;
  protected:
      void setDirty() override;

  public:
      void setProperty(PrimitiveProperty key, std::any value) override;

   protected:
    bool redraw(FrameBuffer &fb) override;
    void redrawBorder(FrameBuffer &fb);
    void drawBackground(FrameBuffer &fb) override;
    const Point valueToPosition(tControlPositionValue range) const;

    const PrimitiveInstance &getPrimitive() const override;

   private:
    Rect m_valueDimension;
    Point m_drawPosition;

    PrimitiveInstance m_primitive;
    Range m_range;
  };
}
