#pragma once

#include "Styles.h"
#include "TemplateEnums.h"

namespace DescriptiveLayouts
{
  class PrimitiveInstance;

  class Styleable
  {
   public:
    Styleable();
    virtual ~Styleable();

    void style(LayoutClasses li, ControlClasses cc, ControlInstances ci);

    virtual void applyStyle(const StyleMap &style);
    int getStyleValue(StyleKey s) const;
    virtual const PrimitiveInstance &getPrimitive() const = 0;

    void setHighlight(bool b)
    {
      m_highlight = b;
    }

    bool isHighlight() const
    {
      return m_highlight;
    }

    static int getDefaultStyleValueForKey(StyleKey s);

   protected:
    virtual void setDirty() = 0;
    bool m_highlight = false;

   private:
    StyleMap m_currentStyle;
  };
}
