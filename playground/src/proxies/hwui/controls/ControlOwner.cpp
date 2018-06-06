#include "ControlOwner.h"
#include "Control.h"
#include "Button.h"

ControlOwner::ControlOwner ()
{
}

ControlOwner::~ControlOwner ()
{
}

const ControlOwner::tControls &ControlOwner::getControls () const
{
  return m_controls;
}

bool ControlOwner::isDirty () const
{
  for (auto c : m_controls)
    if (c->isDirty ())
      return true;

  return false;
}

void ControlOwner::collectDirtyRects(std::list<Rect> &rects) const
{
  for (auto c : m_controls)
    c->collectDirtyRects(rects);
}

void ControlOwner::setDirtyIfOverlapsWithAny(const std::list<Rect> &rects)
{
  for (auto c : m_controls)
    c->setDirtyIfOverlapsWithAny(rects);
}

void ControlOwner::setHighlight (bool isHighlight)
{
  for (auto c : m_controls)
    c->setHighlight (isHighlight);
}

bool ControlOwner::redraw (FrameBuffer &fb)
{
  bool didRedraw = false;

  for (auto c : m_controls)
  {
    if (c->isDirty ())
    {
      c->drawBackground (fb);

      if (c->isVisible ())
        c->redraw (fb);

      c->setClean ();
      didRedraw = true;
    }
  }
  return didRedraw;
}

void ControlOwner::remove (const Control *ctrl)
{
  for (auto it = m_controls.begin (); it != m_controls.end (); it++)
  {
    if (it->get () == ctrl)
    {
      m_controls.erase (it);
      setDirty ();
      return;
    }
  }
}

size_t ControlOwner::getNumChildren () const
{
  return m_controls.size ();
}

void ControlOwner::clear ()
{
  m_controls.clear ();
  setDirty ();
}

void ControlOwner::setAllDirty ()
{
  for (auto c : m_controls)
    c->setDirty ();
}

void ControlOwner::forEach (tIfCallback cb) const
{
  for (auto c : m_controls)
    if (!cb (c))
      return;
}

void ControlOwner::forEach (tCallback cb) const
{
  for (auto c : m_controls)
    cb (c);
}

ControlOwner::tControlPtr ControlOwner::first ()
{
  if (!m_controls.empty ())
    return m_controls.front ();

  return nullptr;
}

bool ControlOwner::isHighlight () const
{
  return false;
}

void ControlOwner::highlight (shared_ptr<Control> c)
{
  c->setHighlight(true);
}

void ControlOwner::noHighlight()
{
  for (auto c : getControls())
    c->setHighlight(false);
}

void ControlOwner::highlightButtonWithCaption (const Glib::ustring &caption)
{
  forEach ([&caption] (tControlPtr ctrl)
  {
    if (auto b = dynamic_pointer_cast<Button> (ctrl))
    {
      if (b->getText () == caption)
      {
        b->setHighlight (true);
      }
      else
      {
        b->setHighlight (false);
      }
    }
  });
}
