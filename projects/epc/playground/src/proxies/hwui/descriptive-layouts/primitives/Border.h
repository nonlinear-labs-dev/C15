#pragma once

#include <proxies/hwui/controls/Control.h>
#include <proxies/hwui/descriptive-layouts/PropertyOwner.h>
#include "proxies/hwui/descriptive-layouts/Styleable.h"
#include "proxies/hwui/descriptive-layouts/PrimitiveInstance.h"

namespace DescriptiveLayouts
{
  class PrimitiveInstance;

  class Border : public Control, public Styleable, public PropertyOwner
  {
   public:
    explicit Border(const PrimitiveInstance &e);
    ~Border() override;
    void setProperty(PrimitiveProperty key, std::experimental::any value) override;

   protected:
    bool redraw(FrameBuffer &fb) override;
    void setDirty() override;
    void drawBackground(FrameBuffer &fb) override;
    const PrimitiveInstance &getPrimitive() const override;

   private:
    PrimitiveInstance m_primitive;
  };
}
