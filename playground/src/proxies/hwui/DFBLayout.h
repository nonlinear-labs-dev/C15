#pragma once

#include "proxies/hwui/Layout.h"
#include "proxies/hwui/controls/ControlOwner.h"
#include <proxies/hwui/buttons.h>

class ButtonRepeat;
class FrameBuffer;
class OLEDProxy;

class DFBLayout : public Layout, public ControlOwner
{
 public:
  DFBLayout(OLEDProxy &oled);
  virtual ~DFBLayout();

  virtual bool redrawLayout() override;

  virtual bool onButton(Buttons i, bool down, ::ButtonModifiers modifiers);
  virtual bool onRotary(int inc, ButtonModifiers modifiers);

  virtual void setDirty() override;

  FrameBuffer &getFrameBuffer();

 protected:
  void installButtonRepeat(std::function<void()> cb);
  void removeButtonRepeat();

  bool isResolutionFine() const;

  OLEDProxy &getOLEDProxy();

 private:
  bool m_clear = true;
  std::unique_ptr<ButtonRepeat> m_buttonRepeat;
  OLEDProxy &m_oled;
};
