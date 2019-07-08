#include <device-settings/DebugLevel.h>
#include <proxies/hwui/DFBLayout.h>
#include <proxies/hwui/FrameBuffer.h>
#include <proxies/hwui/OLEDProxy.h>

OLEDProxy::OLEDProxy(const Rect &posInFrameBuffer)
    : m_posInFrameBuffer(posInFrameBuffer)
{
  Oleds::get().registerProxy(this);
}

OLEDProxy::~OLEDProxy() = default;

const Rect &OLEDProxy::getPosInFrameBuffer() const
{
  return m_posInFrameBuffer;
}

void OLEDProxy::invalidate()
{
  if(auto l = std::dynamic_pointer_cast<DFBLayout>(getLayout()))
    l->setDirty();
  else
    DebugLevel::warning("Oled proxy has NO screen set !??");
}

OLEDProxy::tLayoutPtr OLEDProxy::getLayout() const
{
  if(m_overlay)
    return m_overlay;

  return m_layout;
}

void OLEDProxy::reset(Layout *layout)
{
  tLayoutPtr s(layout);
  reset(s);
}

void OLEDProxy::reset(tLayoutPtr layout)
{
  resetOverlay();

  m_layout = layout;

  if(!layout->isInitialized())
    layout->init();

  DebugLevel::info(G_STRLOC, typeid(layout.get()).name());
  invalidate();
}

void OLEDProxy::setOverlay(Layout *layout)
{
  tLayoutPtr s(layout);
  setOverlay(s);
}

void OLEDProxy::setOverlay(tLayoutPtr layout)
{
  m_overlay = layout;

  if(!layout->isInitialized())
    layout->init();

  DebugLevel::info(G_STRLOC, typeid(layout.get()).name());
  invalidate();
}

void OLEDProxy::resetOverlay()
{
  m_overlay.reset();
  invalidate();
}

bool OLEDProxy::redraw()
{
  bool ret = false;

  if(auto l = getLayout())
  {
    auto &fb = FrameBuffer::get();
    auto clip = fb.clip(m_posInFrameBuffer);
    auto offset = fb.offset(m_posInFrameBuffer.getPosition());
    ret = l->redrawLayout();
  }

  return ret;
}

void OLEDProxy::clear()
{
  auto &fb = FrameBuffer::get();
  fb.setColor(FrameBuffer::Colors::C43);
  fb.fillRect(Rect(0, 0, m_posInFrameBuffer.getWidth(), m_posInFrameBuffer.getHeight()));
}
