#include <proxies/hwui/DFBLayout.h>

class PleaseWaitLayout : public DFBLayout
{
  public:
    PleaseWaitLayout();
    bool onButton(Buttons i, bool down, ButtonModifiers modifiers) override;
};

