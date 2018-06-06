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
      void applyStyle(const StyleMap &style);
      int getStyleValue(StyleKey s) const;
      virtual const PrimitiveInstance &getPrimitive() const = 0;

    protected:
      virtual void setDirty() = 0;

    private:
      StyleMap m_currentStyle;
  };

}
