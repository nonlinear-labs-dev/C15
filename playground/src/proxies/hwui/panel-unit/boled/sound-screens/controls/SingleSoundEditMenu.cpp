#include <Application.h>
#include <libundo/undo/Transaction.h>
#include <libundo/undo/TransactionCreationScope.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/panel-unit/boled/sound-screens/controls/SingleSoundEditMenu.h>
#include <presets/EditBuffer.h>
#include <proxies/hwui/controls/ButtonMenuButton.h>
#include "SingleSoundEditMenu.h"

SingleSoundEditMenu::SingleSoundEditMenu(const Rect &rect)
    : super(rect)
{
  init();
}

void SingleSoundEditMenu::init()
{
  auto pm = Application::get().getPresetManager();

  addButton("Store Init", [=]() {
    auto scope = pm->getUndoScope().startTransaction("Store Init Sound");
    pm->storeInitSound(scope->getTransaction());
    selectButton(0);
  });

  addButton("Reset Init", [=]() {
    auto scope = pm->getUndoScope().startTransaction("Reset Init Sound");
    pm->resetInitSound(scope->getTransaction());
    selectButton(0);
  });

  addButton("Randomize", [=]() {
    auto scope = pm->getUndoScope().startTransaction("Randomize Sound");
    pm->getEditBuffer()->undoableRandomize(scope->getTransaction(), Initiator::EXPLICIT_WEBUI);
  });

  addButton("Mono Mode ..", [] {});
  addButton("Transition Time ..", [] {});
  addButton("Tune Reference ..", [] {});
  selectButton(0);
}

Font::Justification SingleSoundEditMenu::getDefaultButtonJustification() const
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

SoundButton::SoundButton(bool isFirst, bool isLast, const Glib::ustring &text, const Rect &pos)
    : ButtonMenuButton(isFirst, isLast, text, pos)
{
}

void SoundButton::drawBackground(FrameBuffer &fb)
{
  const Rect &r = getPosition();

  if(isHighlight())
  {
    fb.setColor(FrameBuffer::Colors::C128);
    fb.fillRect(r.getX() - 10, r.getTop() + 1, r.getWidth() + 5, r.getHeight() - 3);
  }
  else
  {
    Label::drawBackground(fb);
  }
}
