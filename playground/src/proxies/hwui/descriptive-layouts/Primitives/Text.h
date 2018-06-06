#pragma once

#include "proxies/hwui/descriptive-layouts/LayoutFactory.h"
#include "proxies/hwui/descriptive-layouts/Styleable.h"
#include "proxies/hwui/descriptive-layouts/PropertyOwner.h"
#include "proxies/hwui/descriptive-layouts/PrimitiveInstance.h"
#include <proxies/hwui/controls/Label.h>

namespace DescriptiveLayouts
{
  struct PrimitiveInstance;

  class Text : public Label, public Styleable, public PropertyOwner
  {
      using super = Label;

    public:
      Text(const PrimitiveInstance &e);
      virtual ~Text();

      using DisplayString = std::pair<Glib::ustring, int>;

      void setProperty(PrimitiveProperty key, std::any value);
      void setDirty() override;
      const PrimitiveInstance &getPrimitive() const override;

    protected:
      void setFontColor (FrameBuffer &fb) const override;
      Font::Justification getJustification () const override;
      int getFontHeight () const override;
      void drawBackground (FrameBuffer &fb) override;

    private:
      PrimitiveInstance m_primitive;
  };

}
