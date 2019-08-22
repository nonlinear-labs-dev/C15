#include <proxies/hwui/HWUIEnums.h>
#include "ControlWithChildren.h"

ControlWithChildren::ControlWithChildren(const Rect &pos)
    : super(pos)
{
}

ControlWithChildren::~ControlWithChildren() = default;

bool ControlWithChildren::isDirty() const
{
  if(super::isDirty())
    return true;

  return ControlOwner::isDirty();
}

void ControlWithChildren::setDirty()
{
  Control::setDirty();
}

bool ControlWithChildren::isHighlight() const
{
  return Control::isHighlight();
}

void ControlWithChildren::setHighlight(bool isHighlight)
{
  Control::setHighlight(isHighlight);
  ControlOwner::setHighlight(isHighlight);
}

bool ControlWithChildren::redraw(FrameBuffer &fb)
{
  bool redrew = Control::isDirty();

  if(redrew)
  {
    ControlOwner::setAllDirty();

    if(!isTransparent())
      Control::drawBackground(fb);
  }

  auto clip = fb.clipRespectingOffset(getPosition());

  if(clip.isEmpty())
    return false;

  auto offset = fb.offset(getPosition().getPosition());

  redrew |= ControlOwner::redraw(fb);
  return redrew;
}

void ControlWithChildren::drawBackground(FrameBuffer &fb)
{
}

bool ControlWithChildren::isTransparent() const
{
  return false;
}
