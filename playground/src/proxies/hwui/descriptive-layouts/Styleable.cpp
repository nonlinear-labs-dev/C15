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
    const auto& s = StyleSheet::get();
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

  int Styleable::getDefaultStyleValueForKey(StyleKey s)
  {
    switch(s)
    {
      case StyleKey::Color:
      case StyleKey::HighlightColor:
        return (int) StyleValues::Color::C255;
      case StyleKey::HighlightBackgroundColor:
      case StyleKey::BackgroundColor:
        return (int) StyleValues::Color::C43;
      case StyleKey::SuffixColor:
        return 6;
      case StyleKey::BorderColor:
        return (int) StyleValues::Color::C43;
      case StyleKey::TextAlign:
        return (int) StyleValues::Alignment::Left;
      case StyleKey::FontSize:
        return 9;
      case StyleKey::BorderStyle:
        return (int) StyleValues::BorderStyle::Solid;
      case StyleKey::FontDecoration:
        return (int) StyleValues::Font::Regular;
    }
    return 0;
  }

  int Styleable::getStyleValue(StyleKey s) const
  {
    auto it = m_currentStyle.map.find(s);
    if(it != m_currentStyle.map.end())
    {
      return it->second;
    }
    return getDefaultStyleValueForKey(s);
  }
}
