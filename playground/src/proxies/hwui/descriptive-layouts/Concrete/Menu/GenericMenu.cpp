#include <nltools/logging/Log.h>
#include "GenericMenu.h"
#include "EditorItem.h"

GenericMenu::GenericMenu(const Rect& r)
    : ControlWithChildren(r)
    , m_selection{ 0 }
{
  init();
}

void GenericMenu::init()
{
}

void GenericMenu::bruteForce()
{
  if(m_overlays.empty())
  {
    bruteForceList();
  }
}

void GenericMenu::bruteForceList() const
{
  auto INVALIDPOSITION = Rect(-1000, -1000, 0, 0);

  hideAllControls();

  for(auto& c : m_items)
  {
    auto orderNumber = c->getOrderNumber();
    c->setVisible(std::abs(m_selection - orderNumber) <= 2);
    c->setHighlight(m_selection == orderNumber);

    if(std::abs(m_selection - orderNumber) <= 2)
    {
      c->setPosition(calculatePosition(*c));
    }
    else
    {
      INVALIDPOSITION.setWidth(c->getPosition().getWidth());
      INVALIDPOSITION.setHeight(c->getPosition().getHeight());
      c->setPosition(INVALIDPOSITION);
    }
  }
}

void GenericMenu::hideAllControls() const
{
  for(auto& c : getControls())
  {
    c->setVisible(false);
    c->setHighlight(false);
  }
}

bool GenericMenu::allowWrapAround() const
{
  return true;
}

bool GenericMenu::redraw(FrameBuffer& fb)
{
  if(m_overlays.empty())
  {
    return ControlWithChildren::redraw(fb);
  }
  else
  {
    return m_overlays.top()->redraw(fb);
  }
}

void GenericMenu::selectNext()
{
  auto len = m_items.size();

  if(m_selection + 1 < len)
  {
    m_selection++;
  }
  else if(allowWrapAround())
  {
    m_selection = 0;
  }
}

void GenericMenu::selectPrev()
{
  if(m_selection - 1 >= 0)
  {
    m_selection--;
  }
  else if(allowWrapAround())
  {
    m_selection = static_cast<int>(m_items.size()) - 1;
  }
}

void GenericMenu::enter()
{
  try
  {
    auto& itemPtr = m_items.at(m_selection);
    if(itemPtr->canEnter())
    {
      if(auto editor = dynamic_cast<EditorItem*>(itemPtr.get()))
      {
        m_overlays.emplace(editor->createEditor());
      }
    }
  }
  catch(std::out_of_range& oor)
  {
    nltools::Log::error(oor.what());
    sanitizeIndex();
  }
  catch(...)
  {
    nltools::Log::error("Unexpected Error.. Sanitizing Index");
    sanitizeIndex();
  }
}

void GenericMenu::diveUp()
{
  if(!m_overlays.empty())
  {
    m_overlays.pop();
    setDirty();
  }
}

void GenericMenu::sanitizeIndex()
{
  int maxIndex = static_cast<int>(m_items.size()) - 1;
  m_selection = std::min(maxIndex, std::max(0, m_selection));
}

Rect GenericMenu::calculatePosition(BasicItem& item) const
{
  auto orderNumber = item.getOrderNumber();
  auto diff = orderNumber - m_selection;

  int newY = [&, this]() {
    try
    {
      auto& item = m_items.at(orderNumber - 1);
      auto top = std::max(0, item->getPosition().getTop());
      return top + item->getHeight();
    }
    catch(...)
    {
      return diff * 15;
    }
  }();

  auto current = item.getPosition();
  current.setTop(newY);
  return current;
}

bool GenericMenu::onButton(Buttons i, bool down, ButtonModifiers mod) {
  if(m_overlays.empty()) {
    if(!down)
      return false;

    switch(i) {
      case Buttons::BUTTON_A:
      case Buttons::BUTTON_B:
        diveUp();
        break;
      case Buttons::BUTTON_C:
      case Buttons::BUTTON_D:
        enter();
        break;
    }
  }
  return false;
}
