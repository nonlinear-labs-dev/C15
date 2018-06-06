#pragma once

#include <proxies/hwui/controls/Control.h>
#include "Styleable.h"
#include "PrimitiveInstance.h"

namespace DescriptiveLayouts
{
  class PrimitiveInstance;

  class Border : public Control, public Styleable
  {
    public:
      Border(const PrimitiveInstance &e);
      virtual ~Border();

    protected:
      bool redraw(FrameBuffer &fb) override;
      void setDirty() override;
      void drawBackground(FrameBuffer &fb) override;
      const PrimitiveInstance &getPrimitive() const override;

    private:
      PrimitiveInstance m_primitive;
  };

}
