#include <Application.h>
#include <libundo/undo/Transaction.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SingleSoundEditMenuLegacy.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/controls/ButtonMenuButton.h>
#include "SingleSoundEditMenuLegacy.h"
#include <libundo/undo/Scope.h>

SingleSoundEditMenuLegacy::SingleSoundEditMenuLegacy(const Rect &rect)
    : super(rect)
{
  init();
}

void SingleSoundEditMenuLegacy::init()
{

  addButton("Store Init", [=]() {
    auto pm = Application::get().getPresetManager();
    SoundUseCases soundUseCases(pm->getEditBuffer(), pm);
    soundUseCases.storeInitSound();
    selectButton(0);
  });

  addButton("Reset Init", [=]() {
    auto pm = Application::get().getPresetManager();
    SoundUseCases soundUseCases(pm->getEditBuffer(), pm);
    soundUseCases.resetInitSound();
    selectButton(0);
  });

  addButton("Randomize", [=]() {
    auto pm = Application::get().getPresetManager();
    SoundUseCases soundUseCases(pm->getEditBuffer(), pm);
    soundUseCases.randomizeSound();
  });

  addButton("Mono Mode ..", [] {});
  addButton("Transition Time ..", [] {});
  addButton("Tune Reference ..", [] {});
  selectButton(0);
}

Font::Justification SingleSoundEditMenuLegacy::getDefaultButtonJustification() const
{
  return Font::Justification::Left;
}

SoundMenu::SoundMenu(const Rect &r)
    : ButtonMenu(r, 5)
{
  entryWidth = 128;
}

const size_t c_arrowUp = (size_t) -1;
const size_t c_arrowDown = (size_t) -2;
const size_t c_empty = (size_t) -3;

void SoundMenu::bruteForce()
{
  while(auto p = findControlOfType<ButtonMenuButton>())
    remove(p.get());

  const int buttonHeight = 13;
  int y = 0;

  ButtonMenuButton *selectedButton = nullptr;

  for(size_t i = 0; i < m_numButtonPlaces; i++)
  {
    try
    {
      auto itemToShow = getItemToShowAtPlace(i);

      if(itemToShow != c_empty && itemToShow != c_arrowDown && itemToShow != c_arrowUp)
      {
        Rect buttonPosition(5, y, entryWidth, buttonHeight);
        bool isFirst = (i == 0) || (itemToShow == 0);
        bool isLast = i == (m_numButtonPlaces - 1);
        const Glib::ustring caption = m_items[itemToShow].title;
        auto button = new ButtonMenuButton(false, false, caption, buttonPosition);

        button->setJustification(getDefaultButtonJustification());

        if(itemToShow == m_selected)
          selectedButton = button;
        else
          addControl(button);

        y += (buttonHeight - 1);
      }
    }
    catch(...)
    {
    }
  }

  if(selectedButton)
    addControl(selectedButton)->setHighlight(true);
}
