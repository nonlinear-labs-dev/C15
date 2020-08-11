#include "Layout.h"
#include <Application.h>
#include <proxies/hwui/OLEDProxy.h>
#include <proxies/hwui/Oleds.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/ButtonRepeat.h>
#include <proxies/hwui/HWUI.h>

Layout::Layout(OLEDProxy &oled)
    : m_oled(oled)
{
}

Layout::~Layout() = default;

void Layout::init()
{
  m_initialized = true;
  onInit();
}

void Layout::onInit()
{
}

void Layout::copyFrom(Layout *)
{
}

bool Layout::isInitialized() const
{
  return m_initialized;
}

bool Layout::redrawLayout()
{
  if(m_clear)
  {
    m_clear = false;
    m_oled.clear();
  }

  return redraw(getFrameBuffer());
}

FrameBuffer &Layout::getFrameBuffer()
{
  return FrameBuffer::get();
}

bool Layout::onButton(Buttons i, bool down, ::ButtonModifiers modifiers)
{
  if(i == Buttons::BUTTON_INC || i == Buttons::BUTTON_DEC)
  {
    if(down)
    {
      installButtonRepeat([=]() {
        int direction = (i == Buttons::BUTTON_INC) ? 1 : -1;
        onRotary(direction, modifiers);
      });
    }
    else
    {
      removeButtonRepeat();
    }

    return true;
  }

  return false;
}

bool Layout::onRotary(int inc, ButtonModifiers modifiers)
{
  return false;
}

void Layout::setDirty()
{
  m_clear = true;
  Oleds::get().setDirty();
}

void Layout::installButtonRepeat(std::function<void()> cb)
{
  m_buttonRepeat = std::make_unique<ButtonRepeat>(cb);
}

void Layout::removeButtonRepeat()
{
  m_buttonRepeat.reset();
}

OLEDProxy &Layout::getOLEDProxy()
{
  return m_oled;
}

HWUI *Layout::getHWUI() const
{
  return Application::get().getHWUI();
}