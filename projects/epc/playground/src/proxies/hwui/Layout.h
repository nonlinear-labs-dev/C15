#pragma once

#include "playground.h"
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/buttons.h>
#include <proxies/hwui/controls/ControlOwner.h>
#include <sigc++/trackable.h>

class OLEDProxy;
class ButtonRepeat;
class FrameBuffer;
class OLEDProxy;
class HWUI;

class Layout : public sigc::trackable, public ControlOwner
{
 public:
  explicit Layout(OLEDProxy &oled);
  ~Layout() override;

  virtual bool redrawLayout();
  virtual void init();

  virtual bool onButton(Buttons i, bool down, ::ButtonModifiers modifiers);
  virtual bool onRotary(int inc, ButtonModifiers modifiers);

  virtual void copyFrom(Layout *other);

  void setDirty() override;

  bool isInitialized() const;
  FrameBuffer &getFrameBuffer();

  void installButtonRepeat(std::function<void()> cb);
  void removeButtonRepeat();

 protected:
  virtual void onInit();
  OLEDProxy &getOLEDProxy();
  HWUI *getHWUI() const;

 private:
  bool m_initialized = false;
  bool m_clear = true;
  std::unique_ptr<ButtonRepeat> m_buttonRepeat;
  OLEDProxy &m_oled;
};
