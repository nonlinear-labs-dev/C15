#include <Application.h>
#include <presets/Bank.h>
#include <presets/PresetManager.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <proxies/hwui/controls/Rect.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListBase.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListContent.h>
#include <proxies/hwui/panel-unit/boled/preset-screens/controls/PresetListHeader.h>
#include <functional>
#include <memory>
#include <utility>

PresetListBase::PresetListBase(const Rect &pos, bool showBankArrows)
    : super(pos)
{
  m_header = addControl(new PresetListHeader(Rect(0, 0, 128, 14), showBankArrows));
  m_content = addControl(new PresetListContent(Rect(0, 14, 128, 49)));
  m_content->setHighlight(true);
}

bool PresetListBase::isTransparent() const
{
  return true;
}

PresetListBase::~PresetListBase()
{
}

void PresetListBase::setBankFocus()
{
  m_content->setHighlight(false);
  m_header->setHighlight(true);
}

bool PresetListBase::redraw(FrameBuffer &fb)
{
  Control::drawBackground(fb);

  const Rect &r = getPosition();
  fb.setColor(FrameBufferColors::C103);
  fb.drawRect(r.getLeft(), r.getTop(), r.getWidth(), r.getHeight());

  super::redraw(fb);
  return true;
}

bool PresetListBase::animateSelectedPreset(std::function<void()> cb)
{
  return m_content->animateSelectedPreset(std::move(cb));
}
