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
    DFBLayout ();
    virtual ~DFBLayout ();

    virtual bool redrawLayout (OLEDProxy& oled) override;

    virtual bool onButton (Buttons i, bool down, ::ButtonModifiers modifiers);
    virtual bool onRotary (int inc, ::ButtonModifiers modifiers);

    virtual void setDirty () override;

    FrameBuffer &getFrameBuffer ();

  protected:
    void installButtonRepeat (function<void ()> cb);
    void removeButtonRepeat ();

    bool isResolutionFine () const;
  private:
    bool m_clear = true;
    unique_ptr<ButtonRepeat> m_buttonRepeat;
};

