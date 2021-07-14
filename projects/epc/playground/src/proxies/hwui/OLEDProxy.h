#pragma once

#include "playground.h"
#include <nltools/Uncopyable.h>
#include <proxies/hwui/controls/Rect.h>
#include <memory>
#include <functional>
#include <sigc++/sigc++.h>

class Application;
class Layout;
class FrameBuffer;

class OLEDProxy : public Uncopyable
{
 public:
  explicit OLEDProxy(const Rect &posInFrameBuffer);
  virtual ~OLEDProxy();

  typedef std::shared_ptr<Layout> tLayoutPtr;

  tLayoutPtr getLayout() const;
  tLayoutPtr getOverlay() const;
  tLayoutPtr getBaseLayout() const;

  virtual void reset(Layout *layout);

  virtual void reset(tLayoutPtr layout);

  sigc::connection onLayoutInstalled(const sigc::slot<void, Layout *> &slot);

  void setOverlay(Layout *layout);
  void setOverlay(tLayoutPtr layout);

  void resetOverlay();

  void invalidate();
  void clear();

  virtual bool redraw();

  const Rect &getPosInFrameBuffer() const;

 protected:
  void installScreenSaver(Layout *l);
  void removeScreenSaver();
  Layout *getScreenSaver();

 private:
  sigc::signal<void, Layout *> m_sigLayoutInstalled;

  tLayoutPtr m_layout;
  tLayoutPtr m_overlay;
  tLayoutPtr m_screenSaver;
  Rect m_posInFrameBuffer;
};
