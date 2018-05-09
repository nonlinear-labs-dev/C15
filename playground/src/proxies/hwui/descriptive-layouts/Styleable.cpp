#include "Styleable.h"
#include "PrimitiveInstance.h"
#include "Application.h"
#include <proxies/hwui/HWUI.h>
#include <device-settings/DebugLevel.h>

namespace DescriptiveLayouts
{

  Styleable::Styleable()
  {
  }

  Styleable::~Styleable()
  {
  }

  void Styleable::style(LayoutClasses li, ControlClasses cc, ControlInstances ci)
  {
    const StyleSheet s = StyleSheet::get();
    auto fam = Application::get().getHWUI()->getFocusAndMode();
    const PrimitiveInstance &p = getPrimitive();
    DebugLevel::info("Styling primitive", p.primitiveInstance, "of class", toString(p.primitveClass));

    s.applyStyle(fam.focus, fam.mode, fam.detail, li, cc, ci, p.primitveClass, p.tag, p.primitiveInstance, this);
  }

  void Styleable::applyStyle(const StyleMap &style)
  {
    if(m_currentStyle.map != style.map)
    {
      m_currentStyle = style;
      setDirty();
    }
  }

  int Styleable::getStyleValue(StyleKey s) const
  {
    try
    {
      return m_currentStyle.map.at(s);
    }
    catch(...)
    {
      const PrimitiveInstance &p = getPrimitive();
      DebugLevel::error("Have to define a default for key", toString(s), " for primitive class", toString(p.primitveClass));
      return 0;
    }
  }

}
