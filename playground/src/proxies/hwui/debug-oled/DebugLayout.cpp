#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include "DebugLayout.h"

DebugLayout::DebugLayout(Glib::ustring e) {
  super();
  auto text = addControl(new MultiLineLabel(e));
  auto pos = text->getPosition();
  text->setPosition(Rect(0, 0, 250, 100));
}