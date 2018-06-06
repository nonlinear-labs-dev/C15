#pragma once

#include "LayoutFactory.h"
#include "Styleable.h"
#include "PropertyOwner.h"
#include "PrimitiveInstance.h"
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
