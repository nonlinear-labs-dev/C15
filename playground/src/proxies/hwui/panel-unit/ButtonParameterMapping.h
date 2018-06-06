#pragma once

#include "playground.h"
#include <proxies/hwui/buttons.h>
#include <map>
#include <list>

class ButtonParameterMapping
{
  public:
    ButtonParameterMapping ();
    virtual ~ButtonParameterMapping ();

    void forEachButton(function<void (Buttons, const list<int> &)> cb);
    Buttons findButton(int param) const;
    list<int> findParameters (Buttons button) const;

  private:
    void addMapping (Buttons buttonID, initializer_list<int> parameterIDs);

    map<int, Buttons> m_paramIDToButtonID;
    map<Buttons, list<int>> m_buttonIDToParam;
};

