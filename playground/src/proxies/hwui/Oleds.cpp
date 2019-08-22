#include "Oleds.h"
#include "OLEDProxy.h"
#include "Application.h"
#include <proxies/hwui/Font.h>
#include <proxies/hwui/FrameBuffer.h>
#include <tools/PerformanceTimer.h>

Oleds &Oleds::get()
{
  static Oleds oleds;
  return oleds;
}

Oleds::Oleds()
    : m_throttler(std::chrono::milliseconds(20))
{
}

Oleds::~Oleds()
{
}

void Oleds::setDirty()
{
  if(!m_throttler.isPending())
    m_throttler.doTask(std::bind(&Oleds::syncRedraw, this));
}

bool Oleds::isDirty() const
{
  return m_throttler.isPending();
}

void Oleds::deInit()
{
  m_proxies.clear();
  m_fonts.clear();

  FrameBuffer::get().clear();
  FrameBuffer::get().swapBuffers();
}

void Oleds::registerProxy(OLEDProxy *proxy)
{
  m_proxies.push_back(proxy);
}

void Oleds::syncRedraw()
{
  for(auto proxy : m_proxies)
    proxy->redraw();

  FrameBuffer::get().swapBuffers();
}

Oleds::tFont Oleds::getFont(const Glib::ustring &name, int height)
{
  tKey key(name, height);

  auto it = m_fonts.find(key);

  if(it != m_fonts.end())
    return it->second;

  Glib::ustring path = Application::get().getResourcePath() + name + ".ttf";
  auto font = std::make_shared<Font>(path, height);
  m_fonts[key] = font;
  return font;
}