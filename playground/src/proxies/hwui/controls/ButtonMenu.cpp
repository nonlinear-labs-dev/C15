#include <utility>

#include "ButtonMenu.h"
#include "ButtonMenuButton.h"
#include "ArrowUp.h"
#include "ArrowDown.h"

const size_t c_arrowUp = (size_t) -1;
const size_t c_arrowDown = (size_t) -2;
const size_t c_empty = (size_t) -3;

ButtonMenu::ButtonMenu(const Rect &rect, size_t numButtonPlaces)
    : super(rect)
    , m_selected(0)
    , m_numButtonPlaces(numButtonPlaces)
    , entryWidth(rect.getWidth())
{
}

ButtonMenu::~ButtonMenu()
{
}

void ButtonMenu::setHighlight(bool isHighlight)
{
  if(isHighlight)
  {
    forEach([](tControlPtr c) {
      if(!std::dynamic_pointer_cast<ButtonMenuButton>(c))
        c->setHighlight(true);
    });
  }
  else
  {
    super::setHighlight(false);
  }
}

void ButtonMenu::toggle()
{
  auto i = m_selected;

  i++;

  if(i >= m_items.size())
    i = 0;

  selectButton(i);
  bruteForce();
}

void ButtonMenu::antiToggle()
{
  int i = m_selected;

  i--;

  if(i < 0)
  {
    i = m_items.size() - 1;
  }

  selectButton(i);
  bruteForce();
}

void ButtonMenu::sanitizeIndex()
{
  m_selected = (size_t) sanitizeIndex((int) m_selected);
}

int ButtonMenu::sanitizeIndex(int index)
{
  index = std::min((int) m_items.size() - 1, index);
  index = std::max(0, index);
  return index;
}

size_t ButtonMenu::addButton(const Glib::ustring &caption, Action action)
{
  m_items.push_back({ caption, std::move(action)});
  bruteForce();
  return m_items.size();
}

void ButtonMenu::clearActions()
{
  m_items.clear();
}

void ButtonMenu::doAction()
{
  m_items[m_selected].action();
}

void ButtonMenu::bruteForce()
{
  while(auto p = findControlOfType<ButtonMenuButton>())
    remove(p.get());

  const int buttonHeight = 13;
  int y = 0;

  ButtonMenuButton *selectedButton = nullptr;

  for(size_t i = 0; i < m_numButtonPlaces; i++)
  {
    size_t itemToShow = getItemToShowAtPlace(i);

    if(itemToShow == c_arrowUp)
    {
      addControl(new ArrowUp(Rect(0, y, entryWidth, buttonHeight)));
    }
    else if(itemToShow == c_arrowDown)
    {
      addControl(new ArrowDown(Rect(0, y, entryWidth, buttonHeight)));
    }
    else if(itemToShow != c_empty)
    {
      Rect buttonPosition(0, y, entryWidth, buttonHeight);
      bool isFirst = (i == 0) || (itemToShow == 0);
      bool isLast = i == (m_numButtonPlaces - 1);
      const Glib::ustring &caption = m_items[itemToShow].title;
      auto button = new ButtonMenuButton(isFirst, isLast, caption, buttonPosition);

      button->setJustification(getDefaultButtonJustification());

      if(itemToShow == m_selected)
        selectedButton = button;
      else
        addControl(button);
    }

    y += (buttonHeight - 1);
  }

  if(selectedButton)
    addControl(selectedButton)->setHighlight(true);
}

void ButtonMenu::highlightSelectedButton()
{
  bruteForce();
}

size_t ButtonMenu::getItemToShowAtPlace(size_t place) const
{
  if(m_items.size() <= m_numButtonPlaces)
  {
    size_t startAtPlace = m_numButtonPlaces - m_items.size();

    if(place < startAtPlace)
      return c_empty;

    return place - startAtPlace;
  }

  for(size_t moveUp = 0; moveUp < m_items.size(); moveUp++)
  {
    size_t items[m_numButtonPlaces];

    for(size_t i = 0; i < m_numButtonPlaces; i++)
      items[i] = i + moveUp;

    if(moveUp > 0)
      items[0] = c_arrowUp;

    if(items[m_numButtonPlaces - 1] < m_items.size() - 1)
      items[m_numButtonPlaces - 1] = c_arrowDown;

    for(size_t i = 0; i < m_numButtonPlaces; i++)
      if(items[i] == m_selected)
        return items[place];
  }

  throw std::exception();
}

void ButtonMenu::selectButton(size_t i)
{
  auto index = sanitizeIndex(i);
  if(index != m_selected)
  {
    m_selected = index;
    bruteForce();
  }
}

size_t ButtonMenu::getSelectedButton() const
{
  return m_selected;
}

void ButtonMenu::setItemTitle(size_t i, const Glib::ustring &caption)
{
  if(m_items[i].title != caption)
  {
    m_items[i].title = caption;
    bruteForce();
  }
}

Font::Justification ButtonMenu::getDefaultButtonJustification() const
{
    return Font::Justification::Center;
}


const size_t ButtonMenu::getItemCount() const
{
  return m_items.size();
}
