#include "ScrollMenu.h"
#include "EditorItem.h"

ScrollMenu::ScrollMenu()
    : ControlWithChildren({ 1, 11, 254, 52 })
    , m_numPlaces{ 4 }
{
}

void ScrollMenu::scroll(int direction)
{
  int s = m_items.size();
  int newIndex = m_selectedItem + direction;

  if(newIndex >= s)
  {
    m_selectedItem = static_cast<int>(newIndex - s);
  }
  else if(newIndex < 0)
  {
    m_selectedItem = static_cast<int>(s + newIndex);
  }
  else
  {
    m_selectedItem = std::min<int>(std::max<int>(m_selectedItem + direction, 0), m_items.size() - 1);
  }

  doLayout();
}

bool ScrollMenu::onButton(Buttons i, bool down, ButtonModifiers mod)
{

  if(onButtonOverlay(i, down, mod))
    return true;
  else if(m_overlay != nullptr)
    return false;

  return handleScrolling(i, down) || onSelectedItemButtonHandler(i, down, mod);
}

bool ScrollMenu::onSelectedItemButtonHandler(const Buttons &i, bool down, const ButtonModifiers &mod)
{
  if(m_selectedItem >= m_items.size() || m_selectedItem < 0)
    return false;

  if(auto selectedItem = m_items.at(m_selectedItem))
  {
    if(auto rec = dynamic_cast<ButtonReceiver *>(selectedItem)) {
      if(rec->onButton(i, down, mod)) {
        return true;
      }
    }

    if(down && (i == Buttons::BUTTON_C || i == Buttons::BUTTON_D || i == Buttons::BUTTON_ENTER))
    {
      if(selectedItem->canEnter())
      {
        if(auto enter = dynamic_cast<EditorItem *>(selectedItem))
        {
          if(auto editor = enter->createEditor())
          {
            m_overlay = addControl(editor);
            setDirty();
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool ScrollMenu::onButtonOverlay(const Buttons &i, bool down, const ButtonModifiers &mod)
{
  if(m_overlay)
  {
    if(m_overlay->onButton(i, down, mod))
    {
      return true;
    }
    else if(down && (i == Buttons::BUTTON_A || i == Buttons::BUTTON_B))
    {
      remove(m_overlay);
      m_overlay = nullptr;
      return true;
    }
  }
  return false;
}

bool ScrollMenu::handleScrolling(const Buttons &i, bool down)
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
  return false;
}

void ScrollMenu::doLayout()
{
  std::array<BasicItem *, 4> items{};

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
  for(auto &c : getControls())
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
  for(auto &c : items)
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
