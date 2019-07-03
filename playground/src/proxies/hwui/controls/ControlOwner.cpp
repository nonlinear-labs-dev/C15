#include "ControlOwner.h"
#include "Control.h"
#include "Button.h"

ControlOwner::ControlOwner() = default;

ControlOwner::~ControlOwner() = default;

const ControlOwner::tControls &ControlOwner::getControls() const
{
  return m_controls;
}

bool ControlOwner::isDirty() const
{
  for(const auto &c : m_controls)
    if(c->isDirty())
      return true;

  return false;
}

void ControlOwner::setHighlight(bool isHighlight)
{
  for(const auto &c : m_controls)
    c->setHighlight(isHighlight);
}

bool ControlOwner::redraw(FrameBuffer &fb)
{
  std::vector<tControlPtr> dirtyControls;
  std::vector<tControlPtr> underlyingControls;

  for(const auto &c : m_controls)
  {
    if(c->isDirty())
    {
      forEach((tCallback)[&underlyingControls, &c, &fb](tControlPtr child) {
        if(c.get() != child.get())
        {
          if(c->getPosition().intersects(child->getPosition()))
          {
            underlyingControls.emplace_back(child);
          }
        }
      });

      if(c->isVisible())
      {
        dirtyControls.emplace_back(c);
      }
    }
  }

  std::for_each(dirtyControls.begin(), dirtyControls.end(), [&underlyingControls](auto& e) {
     underlyingControls.emplace_back(e);
  });

  for(const auto &c : underlyingControls)
  {
    c->drawBackground(fb);

    if(c->isVisible())
      c->redraw(fb);

    c->setClean();
  }

  return dirtyControls.size() > 0;
}

void ControlOwner::remove(const Control *ctrl)
{
  for(auto it = m_controls.begin(); it != m_controls.end(); it++)
  {
    if(it->get() == ctrl)
    {
      m_controls.erase(it);
      setDirty();
      return;
    }
  }
}

size_t ControlOwner::getNumChildren() const
{
  return m_controls.size();
}

void ControlOwner::clear()
{
  m_controls.clear();
  setDirty();
}

void ControlOwner::setAllDirty()
{
  for(const auto &c : m_controls)
    c->setDirty();
}

void ControlOwner::forEach(tIfCallback cb) const
{
  for(const auto &c : m_controls)
    if(!cb(c))
      return;
}

void ControlOwner::forEach(tCallback cb) const
{
  for(const auto &c : m_controls)
    cb(c);
}

ControlOwner::tControlPtr ControlOwner::first()
{
  if(!m_controls.empty())
    return m_controls.front();

  return nullptr;
}

bool ControlOwner::isHighlight() const
{
  return false;
}

void ControlOwner::highlight(std::shared_ptr<Control> c)
{
  c->setHighlight(true);
}

void ControlOwner::noHighlight()
{
  for(auto &c : getControls())
    c->setHighlight(false);
}

void ControlOwner::highlightButtonWithCaption(const Glib::ustring &caption)
{
  forEach([&caption](tControlPtr ctrl) {
    if(auto b = std::dynamic_pointer_cast<Button>(ctrl))
    {
      b->setHighlight(b->getText().text == caption);
    }
  });
}
