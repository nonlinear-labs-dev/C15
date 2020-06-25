#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>
#include "SoundEditInfoLayout.h"

SoundEditInfoLayout::SoundEditInfoLayout(SetupInfoEntries entry)
    : ControlWithChildren(Rect(0, 0, 256, 96))
{
  addControl(new Label({ SetupInfoHeaders.at(entry), 0 }, Rect(0, 0, 256, 12)));
  addControl(new MultiLineLabel(Rect(0, 0, 256, 12), SetupInfoContent.at(entry)));
}
