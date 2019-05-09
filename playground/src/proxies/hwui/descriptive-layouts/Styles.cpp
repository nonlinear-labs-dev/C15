#include "Styles.h"
#include "Styleable.h"
#include <proxies/hwui/HWUI.h>

namespace DescriptiveLayouts
{

  StyleSheet::StyleSheet()
  {
  }

  void StyleSheet::registerStyleFull(UIFocus f, UIMode m, UIDetail d, LayoutClasses l, ControlClasses cc,
                                     ControlInstances ci, PrimitiveClasses pc, PrimitiveTag pt, PrimitiveInstances pi,
                                     const StyleMap &s)
  {
    m_styles[f][m][d][l][cc][ci][pc][pt][pi] = s;
  }

  void StyleSheet::applyStyle(UIFocus f, UIMode m, UIDetail d, LayoutClasses l, ControlClasses cc, ControlInstances ci,
                              PrimitiveClasses pc, PrimitiveTag pt, PrimitiveInstances pi, Styleable *target) const
  {
    StyleMap style;
    DebugLevel::info("Collecting styles for", toString(f), toString(m), toString(d), l, cc, ci, toString(pc), pt, pi);
    m_styles.collectStyle(style, f, m, d, l, cc, ci, pc, pt, pi);
    target->applyStyle(style);
  }

  StyleSheet &StyleSheet::get()
  {
    static StyleSheet sheet;
    return sheet;
  }

  void StyleSheet::clear()
  {
    m_styles.clear();
  }
}
