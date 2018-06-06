#pragma once

#include "playground.h"
#include <proxies/hwui/HWUIEnums.h>
#include <proxies/hwui/buttons.h>
#include <tools/Uncopyable.h>
#include <map>
#include <set>

using namespace std;

class Application;

class UsageMode : public Uncopyable
{
  public:
    UsageMode ();
    virtual ~UsageMode ();

    virtual bool onButtonPressed (Buttons buttonID, ButtonModifiers modifiers, bool state);

    virtual void setup () = 0;
    virtual void setupFocusAndMode (FocusAndMode focusAndMode);
    virtual void bruteForceUpdateLeds();

  protected:
    typedef function<bool (Buttons button, ButtonModifiers modifiers, bool state)> tAction;

    void setupButtonConnection (Buttons buttonID, tAction action);

    Application &getApp ();
    const Application &getApp () const;

#if _TESTS
    set<gint32> assignedAudioIDs;
#endif

  private:
    std::map<Buttons, tAction> m_actions;

};
