#include <proxies/hwui/panel-unit/boled/preset-screens/controls/InvertedLabel.h>
#include "SetupInfoLayout.h"

SetupInfoLayout::SetupInfoLayout(SetupInfoEntries entry)
    : ControlWithChildren(Rect(0, 0, 256, 96))
{
  addControl(new InvertedLabel("Info", Rect(192, 0, 64, 12)));
  addControl(new Label({ SetupInfoHeaders.at(entry), 0 }, Rect(64, 0, 128, 12)));
  addControl(new InvertedLabel("Setup", Rect(0, 0, 64, 12)));
  m_content = addControl(new MultiLineLabel(Rect(0, 12, 256, 84), SetupInfoContent.at(entry)));
}
