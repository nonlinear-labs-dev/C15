#pragma once

#include <presets/PresetBank.h>
#include <proxies/hwui/controls/ControlWithChildren.h>
#include <proxies/hwui/panel-unit/boled/parameter-screens/controls/Scrollable.h>
#include <proxies/hwui/panel-unit/boled/InfoContent.h>

class MultiLineLabel;
class Label;
class PresetBank;

class BankInfoContent : public InfoContent
{
 private:
  typedef InfoContent super;

 public:
  BankInfoContent();
  virtual ~BankInfoContent();

  void setPosition(const Rect &rect) override;
  const Rect &getPosition() const override;
  void setDirty() override;

 private:
  void onBankSelectionChanged(shared_ptr<PresetBank> bank);
  void onBankChanged(shared_ptr<PresetBank> bank);

  void fixLayout();
  bool fillFromBank(PresetBank *bank);
  bool fillDefaults();

  sigc::connection m_bankConnection;
};
