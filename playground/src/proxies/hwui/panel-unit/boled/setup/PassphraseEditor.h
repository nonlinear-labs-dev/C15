#pragma once

#include "MenuEditor.h"

class Setting;

class PassphraseEditor : public MenuEditor
{
  private:
    typedef MenuEditor super;

  public:
    PassphraseEditor ();
    virtual ~PassphraseEditor ();

    void incSetting (int inc) override;
    const vector<ustring> &getDisplayStrings () const override;
    int getSelectedIndex () const override;
    bool onButton (Buttons i, bool down, ButtonModifiers modifiers) override;

  private:
    int m_selection = 0;
};

