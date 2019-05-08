#include "Oleds.h"
#include "OLEDProxy.h"
#include "Application.h"
#include <proxies/hwui/Font.h>
#include <proxies/hwui/FrameBuffer.h>

Oleds &Oleds::get()
{
  static Oleds oleds;
  return oleds;
}

Oleds::Oleds()
{
  Glib::MainContext::get_default()->signal_timeout().connect(mem_fun(this, &Oleds::regularRedraw), 50);
}

Oleds::~Oleds()
{
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

bool Oleds::regularRedraw()
{
  syncRedraw();
  return true;
}

void Oleds::syncRedraw()
{
  bool needsSwap = false;

  for(auto proxy : m_proxies)
    needsSwap |= proxy->redraw();

  if(needsSwap)
    FrameBuffer::get().swapBuffers();
}

Oleds::tFont Oleds::getFont(const Glib::ustring &name, int height)
{
  tKey key(name, height);

  auto it = m_fonts.find(key);

  if(it != m_fonts.end())
    return it->second;

  Glib::ustring path = Application::get().getResourcePath() + name + ".ttf";
  auto font = make_shared<Font>(path, height);
  m_fonts[key] = font;
  return font;
}
