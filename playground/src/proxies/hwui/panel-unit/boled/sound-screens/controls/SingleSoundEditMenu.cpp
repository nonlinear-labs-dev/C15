#include <Application.h>
#include <libundo/undo/Transaction.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SingleSoundEditMenu.h>
#include <presets/EditBuffer.h>

SingleSoundEditMenu::SingleSoundEditMenu(const Rect &rect)
    : super(rect)
{
  auto pm = Application::get().getPresetManager();

  addButton("Store Init", [=]() {
    auto scope = pm->getUndoScope().startTransaction("Store Init Sound");
    pm->storeInitSound(scope->getTransaction());
    selectButton(-1);
  });

  addButton("Reset Init", [=]() {
    auto scope = pm->getUndoScope().startTransaction("Reset Init Sound");
    pm->resetInitSound(scope->getTransaction());
    selectButton(-1);
  });

  addButton("Randomize", [=]() {
    auto scope = pm->getUndoScope().startTransaction("Randomize Sound");
    pm->getEditBuffer()->undoableRandomize(scope->getTransaction(), Initiator::EXPLICIT_WEBUI);
  });

  addButton("Mono Mode ..", [] {});

  addButton("Transition Time ..", [] {});

  addButton("Tune Reference ..", [] {});

  selectButton(-1);
}

Font::Justification SingleSoundEditMenu::getDefaultButtonJustification() const
{
  return Font::Justification::Left;
}
