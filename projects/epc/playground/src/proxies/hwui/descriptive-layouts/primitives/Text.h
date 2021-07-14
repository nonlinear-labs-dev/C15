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
    using DisplayString = std::pair<Glib::ustring, int>;

    explicit Text(const PrimitiveInstance &e);
    ~Text() override;

    std::shared_ptr<Font> getFont() const override;
    void setProperty(PrimitiveProperty key, std::experimental::any value) override;
    void setDirty() override;
    const PrimitiveInstance &getPrimitive() const override;

    void applyStyle(const StyleMap &style) override;

   protected:
    void setFontColor(FrameBuffer &fb) const override;
    void setSuffixFontColor(FrameBuffer &fb) const override;

    Font::Justification getJustification() const override;
    int getFontHeight() const override;

    void drawBackground(FrameBuffer &fb) override;
    StringAndSuffix shortenStringIfNeccessary(const std::shared_ptr<Font> &font,
                                              const StringAndSuffix &text) const override;

   private:
    PrimitiveInstance m_primitive;
    mutable std::shared_ptr<Font> m_cachedFont;
  };
}
