#include <proxies/hwui/descriptive-layouts/concrete/menu/menu-items/AnimatedGenericItem.h>
#include "ScrollMenu.h"
#include "proxies/hwui/descriptive-layouts/concrete/menu/menu-items/EditorItem.h"

ScrollMenu::ScrollMenu(const Rect &r)
    : ControlWithChildren(r)
{
}

void ScrollMenu::scroll(int direction)
{
  int s = m_items.size() - 1;
  int newIndex = m_selectedItem + direction;
  m_selectedItem = std::min(s, std::max(newIndex, 0));
  doLayout();
}

bool ScrollMenu::onButton(Buttons i, bool down, ButtonModifiers mod)
{
  if(m_overlay)
  {
    return onButtonOverlay(i, down, mod);
  }

  return handleScrolling(i, down) || onSelectedItemButtonHandler(i, down, mod);
}

bool ScrollMenu::onSelectedItemButtonHandler(const Buttons &i, bool down, const ButtonModifiers &mod)
{
  if(m_selectedItem >= m_items.size() || m_selectedItem < 0)
    return false;

  if(auto selectedItem = m_items.at(m_selectedItem))
  {
    if(down && (i == Buttons::BUTTON_C || i == Buttons::BUTTON_D || i == Buttons::BUTTON_ENTER))
    {
      if(selectedItem->canEnter())
      {
        if(auto enter = dynamic_cast<EditorItem *>(selectedItem))
        {
          if(auto editor = enter->createOverlay())
          {
            m_overlay = addControl(editor);
            doLayout();
            return true;
          }
        }
      }
      else
      {
        selectedItem->doAction();
        return true;
      }
    }
  }
  return false;
}

bool ScrollMenu::onButtonOverlay(const Buttons &i, bool down, const ButtonModifiers &mod)
{
  if(m_overlay)
  {
    if(down && (i == Buttons::BUTTON_A || i == Buttons::BUTTON_B || i == Buttons::BUTTON_ENTER))
    {
      remove(m_overlay);
      m_overlay = nullptr;
      doLayout();
      return true;
    }
    else if(m_overlay->onButton(i, down, mod))
    {
      return true;
    }
  }
  return false;
}

bool ScrollMenu::handleScrolling(const Buttons &i, bool down)
{
  if(down)
  {
    if(i == Buttons::BUTTON_INC || i == Buttons::ROTARY_PLUS)
    {
      scroll(1);
      return true;
    }
    else if(i == Buttons::BUTTON_DEC || i == Buttons::ROTARY_MINUS)
    {
      scroll(-1);
      return true;
    }
  }
  return false;
}

void ScrollMenu::doLayout()
{
  if(m_overlay)
  {
    auto castedMe = dynamic_cast<MenuOverlay *>(this);
    if(castedMe != m_overlay)
    {
      m_overlay->setVisible(true);
      return;
    }
  }

  std::vector<MenuItem *> items;

  for(int i = m_selectedItem - 1; i < m_selectedItem + 3; i++)
  {
    if(i >= 0 && i < m_items.size())
    {
      auto ptr = m_items.at(i);
      ptr->setHighlight(i == m_selectedItem);
      items.emplace_back(ptr);
    }
    else
    {
      items.emplace_back(nullptr);
    }
  }

  for(auto &c : getControls())
  {
    c->setVisible(false);
  }

  auto maxY = 0;
  for(auto &c : items)
  {
    if(c == nullptr)
    {
      maxY += getDefaultItemHeight();
    }
    else
    {
      const auto height = c->getHeight();
      c->setVisible(true);
      c->setPosition({ 0, maxY, c->getWidth(), height });
      maxY += height;
    }
  }

  setDirty();
}

int ScrollMenu::getDefaultItemHeight() const
{
  return 13;
}
