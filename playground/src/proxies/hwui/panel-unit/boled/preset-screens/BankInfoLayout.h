#pragma once

#include <proxies/hwui/panel-unit/boled/parameter-screens/InfoLayout.h>

class PresetBank;

class BankInfoLayout : public InfoLayout
{
  private:
    typedef InfoLayout super;

  public:
    BankInfoLayout ();
    virtual ~BankInfoLayout ();

    virtual bool onButton (Buttons i, bool down, ButtonModifiers modifiers) override;

  protected:
    virtual void addModuleCaption () override;
    virtual void addHeadline () override;
    virtual void addInfoLabel () override;
    virtual Scrollable * createScrollableContent () override;

    void onBankChanged (shared_ptr<PresetBank> bank);
    void onPresetSelectionChanged ();
};

