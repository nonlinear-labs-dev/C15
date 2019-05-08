#include <Application.h>
#include <libundo/undo/Transaction.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SingleSoundEditMenu.h>

SingleSoundEditMenu::SingleSoundEditMenu(const Rect &rect)
    : super(rect)
{
  auto pm = Application::get().getPresetManager();

  addButton("Store Init", [=]() {
    auto scope = pm->getUndoScope().startTransaction("Store Init Sound");
    pm->undoableStoreInitSound(scope->getTransaction());
    selectButton(-1);
  });

  addButton("Reset Init", [=]() {
    auto scope = pm->getUndoScope().startTransaction("Reset Init Sound");
    pm->undoableResetInitSound(scope->getTransaction());
    selectButton(-1);
  });

  selectButton(-1);
}

SingleSoundEditMenu::~SingleSoundEditMenu()
{
}
