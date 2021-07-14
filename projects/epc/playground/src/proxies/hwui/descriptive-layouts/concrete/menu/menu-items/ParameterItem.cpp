#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-overlays/ParameterOverlay.h>
#include "ParameterItem.h"
#include "parameters/Parameter.h"

ParameterItem::ParameterItem(Parameter *const parameter, const Rect &r)
    : EditorItem(parameter->getLongName(), r)
    , m_parameter { parameter }
{
}

MenuOverlay *ParameterItem::createOverlay()
{
  return new ParameterOverlay(getDefaultOverlayRect(), m_parameter);
}
