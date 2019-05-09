#include "RenameLayout.h"
#include "controls/TextEditControl.h"
#include "Application.h"
#include <proxies/hwui/controls/Label.h>
#include <proxies/hwui/controls/Button.h>
#include <proxies/hwui/controls/RenameLetter.h>
#include <proxies/hwui/HWUI.h>
#include "proxies/hwui/TextEditUsageMode.h"

const int c_numCols = 12;
const int c_numRows = 4;
const int c_letterWidth = 256 / 12;
const int c_letterHeight = 12;

RenameLayout::RenameLayout()
    : super(Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled())
{
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();
  m_oldUsageMode = panelUnit.getUsageMode();
}

RenameLayout::~RenameLayout()
{
  Application::get().getHWUI()->getPanelUnit().restoreUsageMode(m_oldUsageMode);
}

void RenameLayout::init()
{
  Layout::init();

  replaceUsageMode();

  addLetters();
  addControlKeys();

  addControl(new Button("Cancel", Buttons::BUTTON_A));
  addControl(new Button("Apply", Buttons::BUTTON_D));

  addControl(new TextEditControl(Rect(67, 51, 122, 11), m_textUsageMode));

  m_textUsageMode->onTextChanged(mem_fun(this, &RenameLayout::onTextChanged));
}

void RenameLayout::replaceUsageMode()
{
  auto &panelUnit = Application::get().getHWUI()->getPanelUnit();
  panelUnit.setUsageMode(new TextEditUsageMode(getInitialText()));
  auto newUsageMode = panelUnit.getUsageMode();
  m_textUsageMode = std::dynamic_pointer_cast<TextEditUsageMode>(newUsageMode);
}

void RenameLayout::addLetters()
{
  int x = 1;

  for(int col = 0; col < c_numCols; col++)
  {
    if(col == c_numCols / 2)
      x += 2;

    int y = 0;

    for(int row = 0; row < c_numRows - 1; row++)
    {
      Buttons buttonID = (Buttons)(col * c_numRows + row);
      Glib::ustring label = m_textUsageMode->getKeyLabel(buttonID);
      addControl(new RenameLetter(m_textUsageMode, buttonID, Rect(x, y, c_letterWidth, c_letterHeight)));
      y += c_letterHeight;
    }

    x += c_letterWidth;
  }
}

void RenameLayout::addControlKeys()
{
  int y = 3 * c_letterHeight;
  int x = 1;
  int twiceLetterWidth = c_letterWidth * 2;

  for(int col = 0; col < c_numCols / 2; col++)
  {
    if(col == c_numCols / 4)
      x += 2;

    Buttons buttonID = (Buttons)(2 * col * c_numRows + 3);
    Glib::ustring label = m_textUsageMode->getKeyLabel(buttonID);
    addControl(new RenameLetter(m_textUsageMode, buttonID, Rect(x, y, twiceLetterWidth, c_letterHeight)));
    x += twiceLetterWidth;
  }
}

void RenameLayout::onTextChanged(const ustring &text)
{
  setDirty();
}

bool RenameLayout::onButton(Buttons i, bool down, ButtonModifiers modifiers)
{
  switch(i)
  {
    case Buttons::BUTTON_A:
      if(down)
      {
        cancel();
        Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().resetOverlay();
      }

      return true;

    case Buttons::BUTTON_D:
    case Buttons::BUTTON_ENTER:
      if(down)
      {
        commit(m_textUsageMode->getText());
        Application::get().getHWUI()->getPanelUnit().getEditPanel().getBoled().resetOverlay();
        return true;
      }
  }

  return super::onButton(i, down, modifiers);
}

bool RenameLayout::onRotary(int inc, ButtonModifiers modifiers)
{
  m_textUsageMode->moveCursor(inc);
  return true;
}

void RenameLayout::cancel()
{
}
