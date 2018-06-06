#include "Styleable.h"
#include "PrimitiveInstance.h"
#include "Application.h"
#include <proxies/hwui/HWUI.h>
#include <device-settings/DebugLevel.h>
#include <proxies/hwui/debug-oled/DebugLayout.h>

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
    DebugLevel::info("Styling primitive ", p.primitiveInstance, "of class ", toString(p.primitveClass));

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

  int getDefaultStyleValueForKey(StyleKey s) {
    return 0;
  }

  int Styleable::getStyleValue(StyleKey s) const
  {
    auto it = m_currentStyle.map.find(s);
    if(it != m_currentStyle.map.end()) {
      return it->second;
    }
    return getDefaultStyleValueForKey(s);
  }

}
