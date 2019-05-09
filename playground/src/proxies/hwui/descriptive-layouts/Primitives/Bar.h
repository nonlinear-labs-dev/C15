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
    Bar(const PrimitiveInstance &e);
    virtual ~Bar();

    using Range = std::pair<tControlPositionValue, tControlPositionValue>;

    void setProperty(PrimitiveProperty key, std::any value) override;

   protected:
    bool redraw(FrameBuffer &fb) override;
    void setDirty() override;
    void drawBackground(FrameBuffer &fb) override;

    const PrimitiveInstance &getPrimitive() const;

   private:
    PrimitiveInstance m_primitive;
    Range m_range;
  };
}
