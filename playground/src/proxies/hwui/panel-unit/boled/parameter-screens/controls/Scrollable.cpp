#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/Scrollable.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/ScrollArea.h>

void Scrollable::notifyDirty(bool b)
{
  m_sigDirty.send(b);
}

sigc::connection Scrollable::onDirtyStateChanged(slot<void, bool> cb)
{
  return m_sigDirty.connect(cb);
}

void Scrollable::setArea(ScrollArea *area)
{
  m_area = area;
}

void Scrollable::scrollTop()
{
  m_area->scroll(std::numeric_limits<int16_t>::min());
}
