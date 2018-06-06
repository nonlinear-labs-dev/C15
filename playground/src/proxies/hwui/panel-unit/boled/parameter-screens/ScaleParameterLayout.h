#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/UnmodulatebaleParameterLayouts.h>

class ScaleParameterSelectLayout : public UnmodulateableParameterSelectLayout2
{
    typedef UnmodulateableParameterSelectLayout2 super;

  public:
    using super::super;

    virtual void init () override;

  protected:
    void addButtons () override;
    bool onButton (Buttons i, bool down, ButtonModifiers modifiers) override;
    void reset();
    void selectParameter (int inc);
};

class ScaleParameterEditLayout : public UnmodulateableParameterEditLayout2
{
    typedef UnmodulateableParameterEditLayout2 super;

  public:
    using super::super;
};

