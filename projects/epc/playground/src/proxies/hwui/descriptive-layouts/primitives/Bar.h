#pragma once

#include <proxies/hwui/controls/Control.h>
#include "proxies/hwui/descriptive-layouts/PropertyOwner.h"
#include "proxies/hwui/descriptive-layouts/Styleable.h"
#include "proxies/hwui/descriptive-layouts/PrimitiveInstance.h"

namespace DescriptiveLayouts
{
  class PrimitiveInstance;

  class Bar : public Control, public Styleable, public PropertyOwner
  {
   public:
    explicit Bar(const PrimitiveInstance &e);
    ~Bar() override;

    using Range = std::pair<tControlPositionValue, tControlPositionValue>;

    void setProperty(PrimitiveProperty key, std::experimental::any value) override;

    void applyStyle(const StyleMap &style) override;

   protected:
    bool redraw(FrameBuffer &fb) override;
    void drawBackground(FrameBuffer &fb) override;

    void setDirty() override;

    const PrimitiveInstance &getPrimitive() const override;

   private:
    PrimitiveInstance m_primitive;
    Range m_range;
  };
}
