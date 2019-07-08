#pragma once

#include "proxies/hwui/descriptive-layouts/LayoutFactory.h"
#include "proxies/hwui/descriptive-layouts/Styleable.h"
#include "proxies/hwui/descriptive-layouts/PropertyOwner.h"
#include "proxies/hwui/descriptive-layouts/PrimitiveInstance.h"
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/controls/LabelRegular8.h>

namespace DescriptiveLayouts
{
  struct PrimitiveInstance;

  class Text : public LabelRegular8, public Styleable, public PropertyOwner
  {
    using super = LabelRegular8;

   public:
    Text(const PrimitiveInstance &e);
    virtual ~Text();

    using DisplayString = std::pair<Glib::ustring, int>;

    std::shared_ptr<Font> getFont() const override;

    void setProperty(PrimitiveProperty key, std::any value);
    void setDirty() override;
    const PrimitiveInstance &getPrimitive() const override;

   protected:
    void setFontColor(FrameBuffer &fb) const override;

    Font::Justification getJustification() const override;
    int getFontHeight() const override;
    void drawBackground(FrameBuffer &fb) override;

   private:
    PrimitiveInstance m_primitive;
  };
}
