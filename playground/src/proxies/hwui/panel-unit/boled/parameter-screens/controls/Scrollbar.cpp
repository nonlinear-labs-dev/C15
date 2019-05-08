#include "Scrollbar.h"
#include "Scrollable.h"

Scrollbar::Scrollbar(Scrollable *content, const Rect &pos)
    : super(pos)
    , m_content(content)
{
  m_content->onDirtyStateChanged(mem_fun(this, &Scrollbar::onClientsDirtyStateChanged));
}

bool Scrollbar::redraw(FrameBuffer &fb)
{
  super::redraw(fb);

  int availHeight = getPosition().getHeight();
  int contentHeight = m_content->getPosition().getHeight();
  int pos = m_content->getPosition().getTop();

  auto maxDiff = availHeight - contentHeight;

  if(pos < maxDiff)
    pos = maxDiff;

  if(pos > 0)
    pos = 0;

  Rect r = getPosition();
  fb.setColor(FrameBuffer::Colors::C128);
  fb.drawRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());

  if(contentHeight <= availHeight)
  {
    fb.fillRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());
  }
  else
  {
    double height = (double) availHeight * r.getHeight() / contentHeight;
    height = std::max(5.0, height);
    double y = (double) -pos / contentHeight * availHeight;
    fb.fillRect(r.getLeft(), y, r.getWidth(), height);
  }
  return true;
}

void Scrollbar::onClientsDirtyStateChanged(bool isDirty)
{
  if(isDirty)
    setDirty();
}
