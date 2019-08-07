#pragma once

#include "playground.h"
#include "proxies/hwui/Oleds.h"
#include <nltools/Uncopyable.h>
#include <proxies/hwui/controls/Rect.h>

class Application;
class Layout;
class FrameBuffer;

class OLEDProxy : public Uncopyable
{
 public:
  OLEDProxy(const Rect &posInFrameBuffer);
  virtual ~OLEDProxy();

  typedef std::shared_ptr<Layout> tLayoutPtr;

  tLayoutPtr getLayout() const;
  void reset(Layout *layout);
  void reset(tLayoutPtr layout);

  void setOverlay(Layout *layout);
  void setOverlay(tLayoutPtr layout);

  void resetOverlay();

  void invalidate();
  void clear();

  virtual bool redraw();

  const Rect &getPosInFrameBuffer() const;

 private:
  tLayoutPtr m_layout;
  tLayoutPtr m_overlay;
  Rect m_posInFrameBuffer;
};
