#pragma once

#include "Styles.h"
#include "TemplateEnums.h"

namespace DescriptiveLayouts
{
  class Primitive;

  class Styleable
  {
    public:
      Styleable();
      virtual ~Styleable();

      using StyleMap = std::map<StyleKey, int>;

      void style(LayoutInstances li, ControlClasses cc, ControlInstances ci);
      void applyStyle(const StyleMap &style);
      int getStyleValue(StyleKey s) const;
      virtual const Primitive &getPrimitive() const = 0;

    protected:
      virtual void setDirty() = 0;

    private:
      StyleMap m_currentStyle;
  };

}
