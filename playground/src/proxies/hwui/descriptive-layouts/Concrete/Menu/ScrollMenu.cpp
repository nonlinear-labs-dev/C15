#include "ScrollMenu.h"

ScrollMenu::ScrollMenu()
    : ControlWithChildren({ 1, 11, 254, 52 })
    , m_numPlaces{ 4 }
{
}

void ScrollMenu::scroll(int direction)
{
  int s = m_items.size();
  int newIndex = m_selectedItem + direction;



  if(newIndex >= s) {
    m_selectedItem = static_cast<int>(newIndex - s);
  } else if(newIndex < 0) {
    m_selectedItem = static_cast<int>(s + newIndex);
  } else {
    m_selectedItem = std::min<int>(std::max<int>(m_selectedItem + direction, 0), m_items.size() - 1);
  }

  doLayout();
}

bool ScrollMenu::onButton(Buttons i, bool down, ButtonModifiers mod)
{
  if(down)
  {
    if(i == Buttons::BUTTON_INC)
    {
      scroll(1);
      return true;
    }
    else if(i == Buttons::BUTTON_DEC)
    {
      scroll(-1);
      return true;
    }
  }

  try
  {
    if(auto selectedItem = m_items.at(m_selectedItem))
    {
      if(auto rec = dynamic_cast<ButtonReceiver*>(selectedItem))
        if(rec->onButton(i, down, mod))
          return true;
    }
  }
  catch(...)
  {
  }

  return false;
}

void ScrollMenu::doLayout()
{
  std::array<BasicItem*, 4> items{};

  //collect items

  int outIndex = 0;
  for(int i = m_selectedItem - 1; i < m_selectedItem + 3; i++)
  {
    try
    {
      auto ptr = m_items.at(i);
      ptr->setHighlight(i == m_selectedItem);
      items.at(outIndex) = ptr;
    }
    catch(...)
    {
      items.at(outIndex) = nullptr;
    }
    outIndex++;
  }

  //Move all Controls away
  for(auto& c : getControls())
  {
    c->setVisible(false);

    auto pos = c->getPosition();
    pos.setTop(-1000);
    pos.setLeft(-1000);
    c->setPosition(pos);
  }

  //reorder items
  auto maxY = 0;
  const auto left = getPosition().getLeft();
  for(auto& c : items)
  {
    if(c != nullptr)
    {
      c->setVisible(true);
      const auto height = c->getHeight();
      c->setPosition({ left, maxY, c->getWidth(), height });
      maxY += height;
    }
    else
    {
      maxY += 13;
    }
  }
}
