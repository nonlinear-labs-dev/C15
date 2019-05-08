#include <Application.h>
#include <libundo/undo/Transaction.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/LabelRegular8.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SingleSoundMenu.h>

SingleSoundMenu::SingleSoundMenu(const Rect &rect)
    : super(rect)
{
  addControl(new LabelRegular8("Convert to:", Rect(0, 16, 64, 12)));
  addButton("Morph", [=]() { selectButton(-1); });

  addButton("Dual", [=]() { selectButton(-1); });

  selectButton(-1);
}

SingleSoundMenu::~SingleSoundMenu()
{
}
