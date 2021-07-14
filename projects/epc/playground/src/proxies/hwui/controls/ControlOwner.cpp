#include "ControlOwner.h"
#include "Control.h"
#include "Button.h"
#include <Application.h>
#include <device-settings/LayoutMode.h>

ControlOwner::ControlOwner() = default;

ControlOwner::~ControlOwner() = default;

const ControlOwner::tControls &ControlOwner::getControls() const
{
  return m_controls;
}

void ControlOwner::setHighlight(bool isHighlight)
{
  for(const auto &c : m_controls)
    c->setHighlight(isHighlight);
}

bool ControlOwner::redraw(FrameBuffer &fb)
{
  bool didRedraw = false;

  std::vector<Control *> sorted {};

  for(const auto &c : m_controls)
  {
    sorted.emplace_back(c.get());
  }

  std::sort(sorted.begin(), sorted.end(),
            [](const Control *a, const Control *b) -> bool { return a->getZPriority() < b->getZPriority(); });

  for(const auto &c : sorted)
  {
    c->drawBackground(fb);

    if(c->isVisible())
      c->redraw(fb);

    didRedraw = true;
  }
  return didRedraw;
}

void ControlOwner::remove(const Control *ctrl)
{
  if(!ctrl)
    return;

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

void ControlOwner::forEach(const tIfCallback &cb) const
{
  for(const auto &c : m_controls)
    if(!cb(c))
      return;
}

void ControlOwner::forEach(const tCallback &cb) const
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

void ControlOwner::highlight(const std::shared_ptr<Control> &c)
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

void ControlOwner::highlightButtonWithCaption(const Glib::ustring &caption, bool desiredHighlight)
{
  forEach([&caption, &desiredHighlight](tControlPtr ctrl) {
    if(auto b = std::dynamic_pointer_cast<Button>(ctrl))
    {
      if(b->getText().text == caption)
        b->setHighlight(desiredHighlight);
    }
  });
}
