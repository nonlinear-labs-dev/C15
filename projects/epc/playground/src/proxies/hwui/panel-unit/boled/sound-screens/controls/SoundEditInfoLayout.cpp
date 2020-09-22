#include "SoundEditInfoLayout.h"
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/MultiLineLabel.h>

SoundEditInfoLayout::SoundEditInfoLayout(SetupInfoEntries entry)
    : ControlWithChildren(Rect(0, 0, 256, 96))
{
  addControl(new Label({ SetupInfoTexts::getInfoHeader(entry), 0 }, Rect(0, 0, 256, 12)));
  addControl(new MultiLineLabel(Rect(0, 0, 256, 12), SetupInfoTexts::getInfoContent(entry)));
}