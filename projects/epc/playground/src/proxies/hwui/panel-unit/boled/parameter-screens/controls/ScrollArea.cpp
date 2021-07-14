#include "ScrollArea.h"
#include "Scrollbar.h"
#include "Scrollable.h"
#include <sigc++/sigc++.h>

static const int c_scrollbarDrawWidth = 4;
static const int c_scrollbarWidth = c_scrollbarDrawWidth + 1;

ScrollArea::ScrollArea(Scrollable *content, const Rect &position)
    : super(position)
    , m_content(content)
{
  content->setArea(this);
  content->setPosition(Rect(2, 0, position.getWidth() - c_scrollbarWidth, content->getPosition().getHeight()));
  addControl(dynamic_cast<Control *>(content));
  addControl(new Scrollbar(
      content, Rect(position.getWidth() - c_scrollbarDrawWidth, 0, c_scrollbarDrawWidth, position.getHeight())));

  scroll(0);

  content->onDirtyStateChanged(sigc::hide(sigc::mem_fun(this, &ScrollArea::setDirty)));
}

void ScrollArea::scroll(int diff)
{
  int pos = m_content->getPosition().getTop();
  pos -= diff;

  auto maxDiff = getPosition().getHeight() - m_content->getPosition().getHeight();

  if(pos < maxDiff)
    pos = maxDiff;

  if(pos > 0)
    pos = 0;

  m_content->setPosition(
      Rect(2, pos, getPosition().getWidth() - c_scrollbarWidth, m_content->getPosition().getHeight()));
}
