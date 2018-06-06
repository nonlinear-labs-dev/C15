#pragma once

#include "playground.h"
#include <proxies/hwui/HWUIEnums.h>
#include <tools/Uncopyable.h>

class OLEDProxy;

class Layout : public sigc::trackable, public Uncopyable
{
  public:
    Layout();
    virtual ~Layout();

    virtual bool redrawLayout(OLEDProxy& oled) = 0;
    virtual void init ();

    virtual void copyFrom (Layout *other);

    bool isInitialized() const;

  protected:
    virtual void onInit ();

  private:
    bool m_initialized = false;
};
